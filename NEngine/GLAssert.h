#pragma once

#include <GL/glew.h>
#include <iostream>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << ") " /*<< gluErrorString(error)*/ << ". " << function <<
			" " << file << ":" << line << std::endl;
		return false;
	}
	return true;
}