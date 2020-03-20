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
	GLCall(glBufferData(GL_ARRAY_BUFFER, totalsize, vertices, GL_STATIC_DRAW));

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, Vertex::OFFSET_POSITION));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, (void*)Vertex::OFFSET_COLOR));

	// Index buffer
	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW));
}

void Mesh::Init(std::vector<Vertex>& vertVector, std::vector<unsigned int>& indicesVector)
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
	//meshopt_optimizeOverdraw(&resultIndices[0], &resultIndices[0], ic, &resultVertices[0].posx,  vertex_count, sizeof(Vertex), 1.05f);

	// 4. Vertex fetch optimization
	meshopt_optimizeVertexFetch(&resultVertices[0], &resultIndices[0], ic, &resultVertices[0], vc, sizeof(Vertex));

	vertVector = resultVertices;
	indicesVector = resultIndices;

	std::cout << ic << " " << indicesVector.size() << std::endl;

	vertices = &vertVector[0];
	indices = &indicesVector[0];

	vertexCount = vertVector.size();
	indexCount = indicesVector.size();
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}
