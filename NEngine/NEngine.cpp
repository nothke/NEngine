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

static unsigned int CreateVertexColorShader()
{
	const std::string vert = R"glsl(

		#version 330 core
		
		in vec4 position; // layout(location = 0) // not needed, apparently
		in vec4 color; // layout(location = 3) 

		out vec4 out_color;
		
		void main(){
			gl_Position = position;
			//gl_FrontColor = color;
			out_color = color;
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

struct Vertex
{
	float posx, posy, posz;
	float colr, colg, colb;

	static const int STRIDE = 3 * sizeof(float) + 3 * sizeof(float);
	static const int OFFSET_POSITION = 0;
	static const int OFFSET_COLOR = 3 * sizeof(float);
};

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

	// Vertex buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	const int VLENGTH = 3;

	Vertex vertices[VLENGTH] = {
		// Positions			Colors
		{-0.5f, -0.5f,	0,		1, 0, 0},
		{0,		0.5f,	0,		1, 0, 0},
		{0.5f, -0.5f,	0,		1, 1, 0}
	};

	const int totalsize = VLENGTH * sizeof(Vertex);
	glBufferData(GL_ARRAY_BUFFER, totalsize, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, Vertex::OFFSET_POSITION);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, (void*)Vertex::OFFSET_COLOR);

	unsigned int shader = CreateVertexColorShader();
	glUseProgram(shader);

	// GAME LOOP
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