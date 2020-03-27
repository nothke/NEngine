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

	//std::vector<vec3> points;
	//std::vector<vec4> colors;
	Shader shader;

	std::vector<Vertex> vertices;

	unsigned int vao;
	unsigned int vbo;
	//unsigned int ibo;

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
		//GLCall(glGenBuffers(1, &vbo));
		//GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));

		//const int totalsize = vertices.size() * sizeof(Vertex);
		//GLCall(glBufferData(GL_ARRAY_BUFFER, totalsize, &vertices[0], GL_STATIC_DRAW));

		// Vertex attributes
		// Position
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));
		// Color
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3)));

		// Index Buffer Object
		//GLCall(glGenBuffers(1, &ibo));
		//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
		//GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));

		// Unbind
		GLCall(glBindVertexArray(0));
		//GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		//std::cout << ibo << " " << vao << std::endl;
	}

	void RecompileShader()
	{
		shader.Recompile();
	}

	void Init(int pointsCapacity = 64)
	{
		CreateLineShader();
		CreateVAO();

		vertices.reserve(pointsCapacity);
	}

	void Line(vec3 start, vec3 end, vec4 color = { 1,0,0,1 })
	{
		vertices.push_back({ start, color });
		vertices.push_back({ end, color });
	}

	void Render(const mat4& vp)
	{
		if (vertices.size() == 0)
			return;

		glClear(GL_DEPTH_BUFFER_BIT);
		shader.Bind();
		shader.SetMatrix("_VP", vp);

		// Select vao
		GLCall(glBindVertexArray(vao));
		// Create buffer
		GLCall(glGenBuffers(1, &vbo));
		// Select buffer
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
		// Set data to buffer
		GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STREAM_DRAW));
		// Draw
		GLCall(glDrawArrays(GL_LINES, 0, vertices.size()));

		// Unbind all
		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		vertices.clear();
	}



	void Shaderr()
	{

	}
}