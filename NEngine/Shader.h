#pragma once
#include <string>

unsigned int CompileShader(unsigned int type, const std::string & source);

unsigned int CreateShader(const std::string & vert, const std::string & frag);

unsigned int CreateVertexColorShader();