#pragma once
#include "GL/glew.h"
#include "GLAssert.h"
#include "glm/glm.hpp"
using namespace glm;

struct FullscreenQuad
{
	unsigned int vao;
	unsigned int vbo;

	float vertices[4 * 6]{
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	FullscreenQuad()
	{
		// Create a VAO
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		// Create a VBO
		GLCall(glGenBuffers(1, &vbo));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));

		const int vertexSize = 4 * sizeof(float);
		const int totalsize = 6 * vertexSize;
		GLCall(glBufferData(GL_ARRAY_BUFFER, totalsize, &vertices, GL_STATIC_DRAW));

		// Vertex attributes
		// Position
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vertexSize, 0));
		// UV
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*)sizeof(vec2)));

		// We have no index buffer since we'll use glDrawArrays instead of Elements

		// Unbind
		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void Bind()
	{
		GLCall(glBindVertexArray(vao));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
	}

	void Unbind()
	{
		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void Draw()
	{
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	}

	~FullscreenQuad()
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}
};