#pragma once
#include <string>
#include <glm/glm.hpp>

unsigned int CompileShader(unsigned int type, const std::string & source);

unsigned int CreateShader(const std::string & vert, const std::string & frag);

void SetProjectionMatrix(unsigned int program, glm::mat4 & matrix);

unsigned int CreateVertexColorShader();