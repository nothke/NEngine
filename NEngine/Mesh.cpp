#include "Mesh.h"
#include "GLAssert.h"
#include "GL/glew.h"
#include "meshoptimizer/meshoptimizer.h"

void AddAtribute(int index, int size, int offset, int type = GL_FLOAT, bool normalized = false)
{
	GLCall(glEnableVertexAttribArray(index));
	GLCall(glVertexAttribPointer(index, size, type, normalized ? GL_TRUE : GL_FALSE, Vertex::STRIDE, (void*)offset));
}

void Mesh::Rebuild()
{
	CreateAttributes();
}

unsigned int vao;

void Mesh::CreateAttributes()
{
	// Vertex Array Object
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	// Vertex buffer
	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));

	const int totalsize = vertexCount * sizeof(Vertex);
	GLCall(glBufferData(GL_ARRAY_BUFFER, totalsize, &vertices[0], GL_STATIC_DRAW));

	// Vertex attributes
	AddAtribute(0, 3, Vertex::OFFSET_POSITION);
	AddAtribute(1, 2, Vertex::OFFSET_UV);
	AddAtribute(2, 3, Vertex::OFFSET_COLOR);

	// Index Buffer Object
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));

	// Unbind
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	//std::cout << ibo << " " << vao << std::endl;
}

void Mesh::FindBounds()
{
	glm::vec3 min(0);
	glm::vec3 max(0);

	for (Vertex v : vertices)
	{
		if (v.posx < min.x) min.x = v.posx;
		if (v.posy < min.y) min.y = v.posy;
		if (v.posz < min.z) min.z = v.posz;

		if (v.posx > max.x) max.x = v.posx;
		if (v.posy > max.y) max.y = v.posy;
		if (v.posz > max.z) max.z = v.posz;
	}

	boundsMin = min;
	boundsMax = max;
}

void Mesh::Bind()
{
	GLCall(glBindVertexArray(vao));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
}

// Doesn't work for now
void Mesh::Simplify(float threshold, float target_error)
{
	//float threshold = 0.2f;
	size_t target_index_count = size_t(indexCount * threshold);
	//float target_error = 1e-2f;

	std::vector<unsigned int> lod(indexCount);
	lod.resize(meshopt_simplify(&lod[0], &indices[0], indexCount, &vertices[0].posx, vertexCount, sizeof(Vertex), target_index_count, target_error));

	// This should not work but it does
	indices = lod;
	indexCount = target_index_count;
}

void Mesh::SimplifySloppy(float threshold)
{
	size_t target_index_count = size_t(indexCount * threshold);

	std::vector<unsigned int> lod(target_index_count);
	lod.resize(meshopt_simplifySloppy(&lod[0], &indices[0], indexCount, &vertices[0].posx, vertexCount, sizeof(Vertex), target_index_count));

	indices = lod;
	indexCount = target_index_count;
}

void Mesh::Init(std::vector<Vertex>& vertVector, std::vector<unsigned int>& indicesVector, bool optimizeVertexCache)
{
	if (optimizeVertexCache)
	{
		// 1. Indexing
		size_t ic = indicesVector.size();
		std::vector<unsigned int> remap(ic);
		size_t vc = meshopt_generateVertexRemap(&remap[0], &indicesVector[0], ic, &vertVector[0], ic, sizeof(Vertex));

		std::vector<unsigned int> resultIndices;
		std::vector<Vertex> resultVertices;

		resultIndices.resize(ic);
		meshopt_remapIndexBuffer(&resultIndices[0], &indicesVector[0], ic, &remap[0]);

		resultVertices.resize(vc);
		meshopt_remapVertexBuffer(&resultVertices[0], &vertVector[0], ic, sizeof(Vertex), &remap[0]);

		// 2. Vertex cache optimization
		meshopt_optimizeVertexCache(&resultIndices[0], &resultIndices[0], ic, vc);

		// 3. Overdraw optimization
		meshopt_optimizeOverdraw(&resultIndices[0], &resultIndices[0], ic, &resultVertices[0].posx, vc, sizeof(Vertex), 1.05f);

		// 4. Vertex fetch optimization
		meshopt_optimizeVertexFetch(&resultVertices[0], &resultIndices[0], ic, &resultVertices[0], vc, sizeof(Vertex));

		// 5. Vertex quantization (skipped)
		// 6. Vertex/index buffer compression (skipped)
		// 7. Triangle strip conversion (skipped)
		// 8. Deinterleaved geometry (skipped)
		// 9. Simplification - moved to method

		vertVector = resultVertices;
		indicesVector = resultIndices;
	}

	vertices = vertVector;
	indices = indicesVector;

	vertexCount = vertices.size();
	indexCount = indices.size();

	CreateAttributes();
	FindBounds();
}