#include "pch.h"
#include "Shader.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>

#define LOG(x) std::cout << x << std::endl

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Log shader error
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		// alloca - allocates on the stack dynamically
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		LOG("Failed to compile shader:");
		LOG(message);

		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(
	const std::string& vert,
	const std::string& frag)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vert);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, frag);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void Shader::SetProjectionMatrix(unsigned int program, const glm::mat4& matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE, &matrix[0][0]);
}

void Shader::Bind()
{
	glUseProgram(program);
}

void Shader::SetFloat(const char * name, const float& f)
{
	SetFloat(program, name, f);
}
void Shader::SetVector(const char * name, const glm::vec4& v)
{
	SetVector(program, name, v);
}
void Shader::SetProjectionMatrix(const glm::mat4 & matrix)
{
	SetProjectionMatrix(program, matrix);
}

void Shader::SetVector(unsigned int program, const char* name, const glm::vec4& v)
{
	glUniform4f(glGetUniformLocation(program, name), v.x, v.y, v.z, v.w);
}
void Shader::SetFloat(unsigned int program, const char * name, const float& f)
{
	glUniform1f(glGetUniformLocation(program, name), f);
}

Shader::Shader(ShaderSource& source)
{
	program = CreateShader(source.vertex, source.fragment);
}

unsigned int Shader::CreateVertexColorShader()
{
	const std::string vert = R"glsl(

		#version 330 core
		
		in vec4 position; // layout(location = 0) // not needed, apparently
		in vec4 color; // layout(location = 3) 

		uniform vec4 _InputColor1;
		uniform vec4 _InputColor2;
		uniform mat4 mvp;
		uniform float _Mult;
		uniform float _Range;

		out vec4 out_color;
		
		void main(){
			gl_Position = mvp * position;
			//gl_FrontColor = color;
			//out_color = color;
			
			float grad = (2 + position.y)*0.2;
			grad += length(position) * _Range;
			grad = clamp(grad * _Mult, 0, 1);

            //vec3 vc = mix(vec3(0.7,0.3,1), vec3(0,1,1), grad);
			vec3 vc = mix(_InputColor1.rgb, _InputColor2.rgb, grad);
			out_color = vec4(vc, 1);
		}

		)glsl";

	const std::string frag = R"glsl(

		#version 330 core
		
		in vec4 out_color;

		layout(location = 0) out vec4 color;
		
		void main(){
			//color = vec4(1.0, 0, 0, 1);
			color = out_color;
		}

		)glsl";

	return CreateShader(vert, frag);
}

void Shader::Delete()
{
	glDeleteProgram(program);
}

ShaderSource ShaderReader::Parse(const std::string& path)
{
	std::ifstream stream(path);

	if (!stream.good())
		LOG("Shader " << path << " not found");

	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	ShaderType type;

	std::string line;
	std::stringstream ss[2];

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vert") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("frag") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}