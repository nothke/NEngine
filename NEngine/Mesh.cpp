#include "Mesh.h"

#define GLEW_STATIC
#include "GL/glew.h"

void Mesh::Bind()
{
	// Vertex buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	const int totalsize = vertexCount * sizeof(Vertex);
	glBufferData(GL_ARRAY_BUFFER, totalsize, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, Vertex::OFFSET_POSITION);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, (void*)Vertex::OFFSET_COLOR);

	// Index buffer
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

void Mesh::Init(std::vector<Vertex>& vertVector, std::vector<unsigned int>& indicesVector)
{
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
