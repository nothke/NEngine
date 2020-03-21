#include "Mesh.h"
#include "GLAssert.h"
#define GLEW_STATIC
#include "GL/glew.h"
#include "meshoptimizer/meshoptimizer.h"

void Mesh::Bind()
{
	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	// Vertex buffer
	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));

	const int totalsize = vertexCount * sizeof(Vertex);
	GLCall(glBufferData(GL_ARRAY_BUFFER, totalsize, &vertices[0], GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, Vertex::OFFSET_POSITION));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, (void*)Vertex::OFFSET_COLOR));

	// Index buffer
	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
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
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}
