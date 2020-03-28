#include "pch.h"
#include "Shader.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include "GLAssert.h"

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

void Shader::Recompile()
{
	program = CreateShader(source.vertex, source.fragment);
	FetchUniforms();
}

void Shader::Bind() const
{
	glUseProgram(program);
}

// Local

void Shader::SetFloat(const char * name, const float& f) const
{
	const int id = uniforms.at(name);
	GLCall(glUniform1f(id, f));
	//SetFloat(program, name, f);
}
void Shader::SetVector(const char * name, const glm::vec4& v) const
{
	const int id = uniforms.at(name);
	GLCall(glUniform4f(id, v.x, v.y, v.z, v.w));
	//SetVector(program, name, v);
}
void Shader::SetVPMatrix(const glm::mat4& matrix) const
{
	const int id = uniforms.at("_VP");
	GLCall(glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(matrix)));
	//SetProjectionMatrix(program, matrix);
}
void Shader::SetMMatrix(const glm::mat4& matrix) const
{
	const int id = uniforms.at("_M");
	GLCall(glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(matrix)));
}
void Shader::SetMatrix(const char* name, const glm::mat4& matrix)
{
	const int id = uniforms[name];
	GLCall(glUniformMatrix4fv(id, 1, GL_FALSE, &matrix[0][0]));
}
void Shader::SetInt(const char * name, int i) const
{
	GLCall(glUniform1i(glGetUniformLocation(program, name), i));
}

// Static

inline void Shader::SetFloat(unsigned int program, const char * name, const float& f)
{
	GLCall(glUniform1f(glGetUniformLocation(program, name), f));
}

inline void Shader::SetVector(unsigned int program, const char* name, const glm::vec4& v)
{
	GLCall(glUniform4f(glGetUniformLocation(program, name), v.x, v.y, v.z, v.w));
}

inline void Shader::SetProjectionMatrix(unsigned int program, const glm::mat4& matrix)
{
	int loc_VP = glGetUniformLocation(program, "_VP");
	GLCall(glUniformMatrix4fv(loc_VP, 1, GL_FALSE, &matrix[0][0]));
}

void Shader::FetchUniforms()
{
	uniforms.clear();

	GLint numUniforms = 0;
	glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);

	const GLenum properties[4] = { GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION };

	for (int unif = 0; unif < numUniforms; ++unif)
	{
		GLint values[4];
		glGetProgramResourceiv(program, GL_UNIFORM, unif, 4, properties, 4, NULL, values);

		// Skip any uniforms that are in a block.
		if (values[0] != -1)
			continue;

		// Get the name. Must use a std::vector rather than a std::string for C++03 standards issues.
		// C++11 would let you use a std::string directly.
		std::vector<char> nameData(values[2]);
		glGetProgramResourceName(program, GL_UNIFORM, unif, nameData.size(), NULL, &nameData[0]);
		std::string name(nameData.begin(), nameData.end() - 1);

		//std::cout << "Found uniform: " << name << " at location " << values[3] << std::endl;

		uniforms[name] = values[3];
	}
}

Shader::Shader() {}

Shader::Shader(ShaderSource& source)
	: source(source)
{
	program = CreateShader(source.vertex, source.fragment);
	FetchUniforms();
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