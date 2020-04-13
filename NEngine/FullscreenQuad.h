#pragma once
#include "GL/glew.h"
#include "GLAssert.h"
#include "glm/glm.hpp"
using namespace glm;

struct FullscreenQuad
{
	unsigned int vao;
	unsigned int vbo;

	struct QuadVertex
	{
		vec2 position;
		vec2 uv;
	};

	QuadVertex vertices[6];

	FullscreenQuad()
	{
		// Create a VAO
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		// Create a VBO
		GLCall(glGenBuffers(1, &vbo));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));

		vertices[0] = { vec2(0, 0), vec2(0, 0) };
		vertices[1] = { vec2(0, 1), vec2(0, 1) };
		vertices[2] = { vec2(1, 0), vec2(1, 0) };

		vertices[3] = { vec2(1, 0), vec2(1, 0) };
		vertices[4] = { vec2(0, 1), vec2(0, 1) };
		vertices[5] = { vec2(1, 1), vec2(1, 1) };

		const int totalsize = 6 * sizeof(QuadVertex);
		GLCall(glBufferData(GL_ARRAY_BUFFER, 0, &vertices, GL_STATIC_DRAW));

		// Vertex attributes
		// Position
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), 0));
		// UV
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)sizeof(vec2)));

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