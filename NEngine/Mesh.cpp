#include "Mesh.h"
#include "GLAssert.h"

#define GLEW_STATIC
#include "GL/glew.h"

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
