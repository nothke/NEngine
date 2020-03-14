#include "pch.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define LOG(x) std::cout << x << std::endl
#define V(x,y,z) glm::vec3(x, y, z)

#define UP glm::vec3(0, 1, 0)
#define FORWARD glm::vec3(0, 0, 1)
#define RIGHT glm::vec3(1, 0, 0)
#define LEFT glm::vec3(-1, 0, 0)

struct Vertex
{
	float posx, posy, posz;
	float colr, colg, colb;

	static const int STRIDE = 3 * sizeof(float) + 3 * sizeof(float);
	static const int OFFSET_POSITION = 0;
	static const int OFFSET_COLOR = 3 * sizeof(float);
};

GLFWwindow* window;

bool KeyPressed(int key)
{
	int state = glfwGetKey(window, key);
	return state == GLFW_PRESS;
}

int main()
{
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

	//GLFWwindow* window;

	window = glfwCreateWindow(800, 600, "NEngine", NULL, NULL);

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
		{-0.5f, -0.5f,	0,		0, 1, 1},
		{0,		0.5f,	0,		1, 0, 1},
		{0.5f, -0.5f,	0,		1, 1, 0}
	};

	const int totalsize = VLENGTH * sizeof(Vertex);
	glBufferData(GL_ARRAY_BUFFER, totalsize, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, Vertex::OFFSET_POSITION);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, (void*)Vertex::OFFSET_COLOR);

	//glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

	glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);


	unsigned int shader = CreateVertexColorShader();
	glUseProgram(shader);

	SetProjectionMatrix(shader, proj);

	float lastFrameTime = 0;

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	//glfwSetKeyCallback(window, KeyCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	float addx = 0;
	float addz = 0;

	double lastMouseX = 0;
	double lastMouseY = 0;

	float rotX = 0;
	float rotY = 0;

	// GAME LOOP
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		// cam
		const float time = glfwGetTime();
		const float dt = time - lastFrameTime;
		//LOG(dt);

		const float t = sin(time);
		const glm::vec3 v = glm::vec3(addx, 0.15f, addz + -1.0f);
		//viewMatrix = glm::rotate(viewMatrix, t * 3.0f, UP);
		auto viewMatrix = proj;
		const float mouseSensitivity = 0.005f;
		viewMatrix = glm::rotate(viewMatrix, rotY * mouseSensitivity, RIGHT);
		viewMatrix = glm::rotate(viewMatrix, -rotX * mouseSensitivity, UP);
		viewMatrix = glm::translate(viewMatrix, v);
		SetProjectionMatrix(shader, viewMatrix);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);

		glfwPollEvents();

		if (KeyPressed(GLFW_KEY_A))
			addx += 1 * dt;

		if (KeyPressed(GLFW_KEY_D))
			addx -= 1 * dt;

		if (KeyPressed(GLFW_KEY_W))
			addz += 1 * dt;

		if (KeyPressed(GLFW_KEY_S))
			addz -= 1 * dt;

		if (KeyPressed(GLFW_KEY_ESCAPE))
			break;

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		//lastMouseX -= xpos;
		//lastMouseY -= ypos;

		rotX = (float)xpos;
		rotY = (float)ypos;

		//LOG(xpos);
		addx = xpos * 0.001f;

		lastFrameTime = time;
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}