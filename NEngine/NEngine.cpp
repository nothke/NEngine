#include "pch.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define LOG(x) std::cout << x << std::endl

static unsigned int CompileShader(unsigned int type, const std::string& source)
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

static unsigned int CreateShader(
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

int main()
{
	LOG("First line!");

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

	GLFWwindow* window;

	window = glfwCreateWindow(640, 480, "NEngine", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		LOG("Creating window failed");
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		LOG("GLEW not inited correctly");
		return -1;
	}

	LOG(glGetString(GL_VERSION));

	// Tri buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	const int VERT_STRIDE = 2 * sizeof(float);
	const int VERT_OFFSET_POSITION = 0;

	float positions[] = {
		-0.5f, -0.5f,
		0, 0.5f,
		0.5f, -0.5f
	};

	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VERT_STRIDE, VERT_OFFSET_POSITION);

	const std::string vert = R"glsl(

		#version 330 core
		
		layout(location = 0) in vec4 position;
		
		void main(){
		   gl_Position = position;
		}

		)glsl";

	const std::string frag = R"glsl(

		#version 330 core
		
		layout(location = 0) out vec4 color;
		
		void main(){
		   color = vec4(1.0, 0, 0, 1);
		}

		)glsl";

	unsigned int shader = CreateShader(vert, frag);
	glUseProgram(shader);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}