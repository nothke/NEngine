#pragma once
#include <iostream>
#include "glm/glm.hpp"
using namespace glm;

void Log(vec3 v)
{
	std::cout << v[0] << ", " << v[1] << ", " << v[2] << std::endl;
}

void Log(vec2 v)
{
	std::cout << v[0] << ", " << v[1] << std::endl;
}