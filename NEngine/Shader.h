#pragma once
#include <string>
#include <glm/glm.hpp>

unsigned int CompileShader(unsigned int type, const std::string & source);

unsigned int CreateShader(const std::string & vert, const std::string & frag);

void SetProjectionMatrix(unsigned int program, glm::mat4 & matrix);

namespace Shader
{
	void SetVector(unsigned int program, const char* name, glm::vec4& v);
	void SetFloat(unsigned int program, const char* name, float f);
}

unsigned int CreateVertexColorShader();