#pragma once

#include "AssetManager.h"
#include "glm/glm.hpp"
#include "GLAssert.h"
#include "Shader.h"
using namespace glm;

namespace DebugDraw
{
	std::vector<vec3> points;
	std::vector<vec4> colors;
	Shader shader;

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

	void RecompileShader()
	{
		shader.Recompile();
		//CreateLineShader();
	}

	void Init(int pointsCapacity = 64)
	{
		CreateLineShader();

		points.reserve(pointsCapacity);
		colors.reserve(pointsCapacity / 2);
	}

	void Line(vec3 start, vec3 end, vec4 color = { 1,0,0,1 })
	{
		points.push_back(start);
		points.push_back(end);
		colors.push_back(color);
	}

	void Render(const mat4& vp)
	{
		if (points.size() == 0)
			return;

		glClear(GL_DEPTH_BUFFER_BIT);
		shader.Bind();
		shader.SetMatrix("_VP", vp);

		// Note: GLCall must not be used in glBegin/End block!

		glBegin(GL_LINES);

		for (size_t i = 0; i < colors.size(); i++)
		{
			glColor3fv(&colors[i][0]);
			glVertex3fv(&points[i * 2][0]);
			glVertex3fv(&points[i * 2 + 1][0]);
		}

		glEnd();

		points.clear();
	}



	void Shaderr()
	{

	}
}