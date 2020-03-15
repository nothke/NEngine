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
//#include <imgui/imgui.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#if defined(WIN32) && !defined(USE_CONSOLE)
#include <windows.h>
#include "NEngine.h"
#endif

//#define USE_CONSOLE // When changing this you also need to set Linker > System > SubSystem to Console
#define WINDOWED

#define LOG(x) std::cout << x << std::endl
#define LOGV(x) std::cout << x[0] << ", " << x[1] << ", " << x[2] << std::endl

GLFWwindow* window;

bool mouseView = true;

void LockMouse(bool b)
{
	if (b)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_LEFT_CONTROL:

			mouseView = !mouseView;

			if (mouseView)
				LockMouse(true);
			else LockMouse(false);

			break;
		}
	}
}

bool KeyPressed(int key)
{
	int state = glfwGetKey(window, key);
	return state == GLFW_PRESS;
}

glm::vec4 FromImVec(ImVec4 vec)
{
	glm::vec4 v{ vec.x, vec.y, vec.z, vec.w };
	return v;
}

#if defined(WIN32) && !defined(USE_CONSOLE)
int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd)
#else
int main()
#endif
{
	if (!glfwInit())
		return -1;

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

#ifdef WINDOWED
	const float screenWidth = 800;
	const float screenHeight = 600;
#else
	const float screenWidth = mode->width;
	const float screenHeight = mode->height;
#endif

	const float screenAspectRatio = screenWidth / screenHeight;

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

#ifdef WINDOWED
	window = glfwCreateWindow(screenWidth, screenHeight, "NEngine", NULL, NULL);
#else
	window = glfwCreateWindow(screenWidth, screenHeight, "NEngine", monitor, NULL);
#endif

	if (!window)
	{
		glfwTerminate();
		LOG("Creating window failed");
		return -1;
	}

	//glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		LOG("GLEW not inited correctly");
		return -1;
	}

	LOG(glGetString(GL_VERSION));

	// needed for imgui
	const char* glsl_version = "#version 130";

	// imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	//(char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
	ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Get vertices and indices from file
	ModelReader mr; // TODO: Remove class
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
	glfwSetKeyCallback(window, key_callback);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	LockMouse(true);

	const float mouseSensitivity = 0.2f;

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

	// Data passed to shader
	ImVec4 color1{ 0.7f, 0.3f, 1.0f, 1.0f };
	ImVec4 color2{ 0.0f, 1.0f, 1.0f, 1.0f };
	float shader_mult = 0.3f;
	float shader_range = 1;

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

		if (mouseView)
		{
			rotX += (float)mouseDeltaX * mouseSensitivity * dt;
			rotY += (float)mouseDeltaY * mouseSensitivity * dt;
		}

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

		glm::vec4 inputColor1 = FromImVec(color1);
		Shader::SetVector(shader, "_InputColor1", inputColor1);

		glm::vec4 inputColor2 = FromImVec(color2);
		Shader::SetVector(shader, "_InputColor2", inputColor2);

		Shader::SetFloat(shader, "_Mult", shader_mult);
		Shader::SetFloat(shader, "_Range", shader_range);

		// imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui::ShowDemoWindow();

		{
			ImGui::Begin("So Pro");

			ImGui::ColorEdit3("color 1", (float*)&color1);
			ImGui::ColorEdit3("color 2", (float*)&color2);
			ImGui::SliderFloat("Mult", &shader_mult, 0, 2);
			ImGui::SliderFloat("Range", &shader_range, 0, 2);
			ImGui::Text("DT: %.3f ms, FPS: %.1f, AVG: %.1f", dt, dt * 60 * 60, ImGui::GetIO().Framerate);

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}