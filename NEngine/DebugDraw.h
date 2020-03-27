#pragma once

#include "AssetManager.h"
#include "glm/glm.hpp"
#include "GLAssert.h"
#include "Shader.h"
using namespace glm;

namespace DebugDraw
{
	struct Vertex
	{
		vec3 position;
		vec4 color;
	};

	Shader shader;

	std::vector<Vertex> vertices;

	unsigned int vao;
	unsigned int vbo;

	void CreateLineShader()
	{
		const std::string vert = R"glsl(

		#version 330 core
		
		in vec4 position; // layout(location = 0) // not needed, apparently
		in vec4 color; // layout(location = 3)
		uniform mat4 _VP;

		out vec4 out_color;
		
		void main(){
			gl_Position = _VP * position;
			out_color = color;
		}

		)glsl";

		const std::string frag = R"glsl(

		#version 330 core
		
		in vec4 out_color;

		layout(location = 0) out vec4 color;
		
		void main(){
			color = out_color;
			//color = vec4(1,0,0,1);
		}

		)glsl";

		ShaderSource source;
		source.vertex = vert;
		source.fragment = frag;

		shader = Shader(source);
	}

	void CreateVAO()
	{
		// Vertex Array Object
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		// Vertex buffer
		GLCall(glGenBuffers(1, &vbo));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));

		// You MUST create a buffer (and before attributes), even if it's empty
		GLCall(glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STREAM_DRAW));

		// Vertex attributes
		// Position
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));
		// Color
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3)));

		// We have no index buffer since we'll use glDrawArrays instead of Elements

		// Unbind
		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	// Use when changing resolution
	void RecompileShader()
	{
		shader.Recompile();
		CreateVAO();
	}

	void Init(int pointsCapacity = 64)
	{
		CreateLineShader();
		CreateVAO();

		vertices.reserve(pointsCapacity);
	}

	void Render(const mat4& vp)
	{
		if (vertices.size() == 0)
			return;

		// Clear depth buffer so lines always draw on top
		glClear(GL_DEPTH_BUFFER_BIT);

		shader.Bind();
		shader.SetMatrix("_VP", vp);

		// Select vao
		GLCall(glBindVertexArray(vao));
		// Select buffer
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		// Set data to buffer
		GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STREAM_DRAW));
		// Draw
		GLCall(glDrawArrays(GL_LINES, 0, vertices.size()));

		// Unbind all
		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

		vertices.clear();
	}

	// DRAWING METHODS

	inline void p(const vec3& v, const vec4& c)
	{
		vertices.push_back({ v, c });
	}

	void Line(const vec3& start, const  vec3& end, const vec4& color = { 1, 0, 0, 1 })
	{
		vertices.push_back({ start, color });
		vertices.push_back({ end, color });
	}

	void Ray(const vec3& start, const vec3& dir, const vec4& color = { 1, 0, 0, 1 })
	{
		vertices.push_back({ start, color });
		vertices.push_back({ start + dir, color });
	}

	void AABB(const vec3& center, const vec3& extents, const vec4& color = { 1, 0, 0, 1 })
	{
		// 8 corners
		const vec3 h = extents * 0.5f;
		vec3 p000{ center.x - h.x, center.y - h.y, center.z - h.z };
		vec3 p001{ center.x - h.x, center.y - h.y, center.z + h.z };
		vec3 p010{ center.x - h.x, center.y + h.y, center.z - h.z };
		vec3 p011{ center.x - h.x, center.y + h.y, center.z + h.z };

		vec3 p100{ center.x + h.x, center.y - h.y, center.z - h.z };
		vec3 p101{ center.x + h.x, center.y - h.y, center.z + h.z };
		vec3 p110{ center.x + h.x, center.y + h.y, center.z - h.z };
		vec3 p111{ center.x + h.x, center.y + h.y, center.z + h.z };

		// forward aligned lines
		p(p000, color); p(p001, color);
		p(p010, color); p(p011, color);
		p(p100, color); p(p101, color);
		p(p110, color); p(p111, color);

		// right aligned lines
		p(p000, color); p(p100, color);
		p(p001, color); p(p101, color);
		p(p010, color); p(p110, color);
		p(p011, color); p(p111, color);

		// up aligned lines
		p(p000, color); p(p010, color);
		p(p001, color); p(p011, color);
		p(p100, color); p(p110, color);
		p(p101, color); p(p111, color);
	}

	void Circle(const vec3& center, const float& radius, const vec3& direction, const int interpolations = 32, const vec4& color = { 1, 0, 0, 1 })
	{
		const vec3 dir = normalize(direction);
		// Check if direction is up so we don't have invalid cross product
		const vec3 UP = dir.y == 0 || dir.y == -1 ? vec3(0, 0, 1) : vec3(0, 1, 0);
		const float PI = pi<float>();

		vec3 p1 = normalize(cross(dir, UP));

		float mult = PI * 2.0f / interpolations;
		quat rot = angleAxis(mult, dir);

		for (size_t i = 0; i <= interpolations; i++)
		{
			vec3 p2 = rot * p1;

			if (i > 0)
			{
				p(center + p1, color);
				p(center + p2, color);
			}

			p1 = p2;
		}
	}
}