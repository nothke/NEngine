#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader
{
	unsigned int program;

public:
	void Bind();

	void SetFloat(const char* name, float f);
	void SetVector(const char* name, glm::vec4& v);
	void SetProjectionMatrix(glm::mat4 & matrix);

	// statics
	static void SetVector(unsigned int program, const char* name, glm::vec4& v);
	static void SetFloat(unsigned int program, const char* name, float f);
	static void SetProjectionMatrix(unsigned int program, glm::mat4 & matrix);

	static unsigned int CompileShader(unsigned int type, const std::string & source);
	static unsigned int CreateShader(const std::string & vert, const std::string & frag);
	static unsigned int CreateVertexColorShader();

	Shader(std::string& source);
	Shader();
};

struct ShaderSource
{
	std::string vertex;
	std::string fragment;
};

namespace ShaderReader
{
	ShaderSource ParseShader(const std::string& path);
}

