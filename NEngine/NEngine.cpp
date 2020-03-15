#include "pch.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ModelReader.h"
#include "Vertex.h"
#include <vector>

//#define WINDOWED

#define LOG(x) std::cout << x << std::endl
#define LOGV(x) std::cout << x[0] << ", " << x[1] << ", " << x[2] << std::endl

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

	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

#ifdef WINDOWED
	const float screenWidth = 800;
	const float screenHeight = 600;
#else
	const float screenWidth = mode->width;
	const float screenHeight = mode->height;
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
#endif

	const float screenAspectRatio = screenWidth / screenHeight;

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//GLFWwindow* window;

	window = glfwCreateWindow(screenWidth, screenHeight, "NEngine", NULL, NULL);

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

	// Get vertices and indices from file
	ModelReader mr;
	std::vector<unsigned int> indicesVector;
	std::vector<Vertex> vertVector;
	if (mr.Get("../suz.ply", vertVector, indicesVector) != 0)
		return -1;

	const int VLENGTH = vertVector.size();
	const int ILENGTH = indicesVector.size();

	// Convert vector to array:
	Vertex* vertices = &vertVector[0];
	unsigned int* indices = &indicesVector[0];

	// Vertex buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	const int totalsize = VLENGTH * sizeof(Vertex);
	glBufferData(GL_ARRAY_BUFFER, totalsize, vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, Vertex::OFFSET_POSITION);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, Vertex::STRIDE, (void*)Vertex::OFFSET_COLOR);

	// Index buffer
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ILENGTH * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// Shader
	unsigned int shader = CreateVertexColorShader();
	glUseProgram(shader);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(60.0f / 255, 195.0f / 255, 1, 1);

	// FPS input setup
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	const float mouseSensitivity = 0.005f;

	float addx = 0;
	float addz = 0;

	double lastMouseX = 0;
	double lastMouseY = 0;

	float rotX = 0;
	float rotY = 0;

	double lastMousePosX = 0;
	double lastMousePosY = 0;
	glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

	// Timing
	float lastFrameTime = 0;

	// Matrix
	glm::mat4 proj = glm::perspective(glm::radians(90.0f), screenAspectRatio, 0.1f, 1000.0f);

	glm::vec3 camPos = glm::vec3();

	const glm::vec3 RIGHT = glm::vec3(1, 0, 0);
	const glm::vec3 UP = glm::vec3(0, 1, 0);
	const glm::vec3 FORWARD = glm::vec3(0, 0, 1);

	// GAME LOOP
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Time
		const float time = glfwGetTime();
		const float dt = time - lastFrameTime;

		// Input
		glfwPollEvents();

		addx = KeyPressed(GLFW_KEY_A) ? -1 : KeyPressed(GLFW_KEY_D) ? 1 : 0;
		addz = KeyPressed(GLFW_KEY_W) ? -1 : KeyPressed(GLFW_KEY_S) ? 1 : 0;

		if (KeyPressed(GLFW_KEY_ESCAPE))
			break;

		double mousePosX, mousePosY;
		glfwGetCursorPos(window, &mousePosX, &mousePosY);

		double mouseDeltaX = mousePosX - lastMousePosX;
		double mouseDeltaY = mousePosY - lastMousePosY;

		lastMousePosX = mousePosX;
		lastMousePosY = mousePosY;

		rotX += (float)mouseDeltaX * mouseSensitivity;
		rotY += (float)mouseDeltaY * mouseSensitivity;
		const float rad90 = 1.5708f;
		rotY = glm::clamp(rotY, -rad90, rad90);

		lastFrameTime = time;

		// Camera
		glm::mat4 viewMatrix = glm::mat4(1.0f);
		
		viewMatrix = glm::rotate(viewMatrix, rotY, RIGHT);
		viewMatrix = glm::rotate(viewMatrix, rotX, UP);

		const glm::mat4 inv = glm::inverse(viewMatrix);
		const glm::vec3 right = -glm::normalize(inv[0]);
		glm::vec3 forward = -glm::normalize(inv[2]);
		forward.y = 0;
		forward = glm::normalize(forward);

		camPos += (forward * addz + right * addx) * dt;
		//const glm::vec3 v = forward * addz * dt; //glm::vec3(addx, 0.15f, addz - 1.0f);
		//LOGV(forward);
		viewMatrix = glm::translate(viewMatrix, camPos);
		glm::mat4 mvpMatrix = proj * viewMatrix;
		SetProjectionMatrix(shader, mvpMatrix);

		// Draw call
		glDrawElements(GL_TRIANGLES, ILENGTH, GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}