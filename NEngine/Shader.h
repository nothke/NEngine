#pragma once
#include <string>
#include <glm/glm.hpp>

struct ShaderSource
{
	std::string vertex;
	std::string fragment;
};

class Shader
{
	unsigned int program;

	// statics
	static void SetVector(unsigned int program, const char* name, const glm::vec4& v);
	static void SetFloat(unsigned int program, const char* name, const float& f);
	static void SetProjectionMatrix(unsigned int program, const glm::mat4 & matrix);


	static unsigned int CompileShader(unsigned int type, const std::string & source);
	static unsigned int CreateShader(const std::string & vert, const std::string & frag);

public:
	void SetFloat(const char* name, const float& f) const;
	void SetVector(const char* name, const glm::vec4& v) const;
	void SetVPMatrix(const glm::mat4 & matrix) const;
	void SetMMatrix(const glm::mat4 & matrix) const;

	void Bind() const;

	static unsigned int CreateVertexColorShader();

	void Delete();

	Shader(ShaderSource& source);
};

namespace ShaderReader
{
	ShaderSource Parse(const std::string& path);
}

