#include "pch.h"
#include <iostream>
//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ModelReader.h"
#include "Vertex.h"
#include <vector>
#include "imgui/imgui.h"
#include <chrono>

#include "Application.h"
#include "Renderer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Conversion.h"
#include "Log.h"
#include "Model.h"
#include "instrumentor.h"
#include "GUI.h"

#define USE_CONSOLE // When changing this you also need to set Linker > System > SubSystem to Console/Windows
#if defined(WIN32) && !defined(USE_CONSOLE)
#include <windows.h>
#endif

#define USE_GUI

#define LOG(x) std::cout << x << std::endl
#define LOGV(x) std::cout << x[0] << ", " << x[1] << ", " << x[2] << std::endl
#define LOGV2(x) std::cout << x[0] << ", " << x[1] << std::endl

bool mouseView = true;

Application app;
Renderer renderer;
Camera camera;

std::vector<Shader> shaders;
std::vector<Mesh> meshes;

glm::ivec2 targetResolution = { 1024, 768 };

float cameraSpeedInput = 1;
float cameraSpeed = 1;

bool quitKeyPressed = false;

void LockMouse(bool b)
{
	if (b)
	{
		glfwSetInputMode(app.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(app.window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	else
	{
		glfwSetInputMode(app.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		if (glfwRawMouseMotionSupported())
			glfwSetInputMode(app.window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
	}
}

// forward declaration for key_callback
void RebuildEverything();

// Keyboard button press
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

		case GLFW_KEY_ENTER:
			app.ToggleFullscreen();
			RebuildEverything();

			break;
		case GLFW_KEY_ESCAPE:
			quitKeyPressed = true;

			break;
		}
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset != 0)
	{
		cameraSpeedInput += (float)(yoffset * 0.1f);
		cameraSpeed = exp(cameraSpeedInput);
	}
}

void InitInputCallbacks()
{
	//glfwSetInputMode(gameWindow.window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetKeyCallback(app.window, key_callback);
	glfwSetScrollCallback(app.window, scroll_callback);
}

void RebuildEverything()
{
	renderer.Init();

	for (Shader& shader : shaders)
	{
		auto source = ShaderReader::Parse("../NEngine/res/vertexcolor.glsl");
		shader = Shader(source);
		shader.Bind();
	}

	// Commenting this out locks app
	for (Mesh& mesh : meshes)
		mesh.Bind();

	InitInputCallbacks();

	camera.SetProjection(90.0f, app.aspectRatio);

#ifdef USE_GUI
	GUI::Shutdown();
	//ImGui_ImplOpenGL3_Shutdown();
	//ImGui_ImplGlfw_Shutdown();

	GUI::Init(app.window);
	//ImGui_ImplGlfw_InitForOpenGL(app.window, true);
	//ImGui_ImplOpenGL3_Init((char *)glGetString(GL_NUM_SHADING_LANGUAGE_VERSIONS));
#endif
}

bool KeyPressed(int key)
{
	int state = glfwGetKey(app.window, key);
	return state == GLFW_PRESS;
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
	// TODO: Parse command line arguments

	Instrumentor::Instance().beginSession("Game Session", "../results.json");

	if (app.Init()) return -1;

#ifdef USE_GUI
	GUI::Init(app.window);
#endif

	// Get vertices and indices from file
	std::vector<unsigned int> indicesVector;
	std::vector<Vertex> vertVector;
	if (ModelReader::Get("../suza.ply", vertVector, indicesVector) != 0)
		return -1;

	// Mesh
	Mesh mesh;
	mesh.Init(vertVector, indicesVector);
	mesh.Bind();
	meshes.push_back(mesh);

	// Shader
	auto source = ShaderReader::Parse("../NEngine/res/vertexcolor.glsl");
	Shader shader = Shader(source);
	shaders.push_back(shader);

	shader.Bind();

	renderer.Init();

	// FPS input setup
	InitInputCallbacks();

	LockMouse(true);

	const float mouseSensitivity = 0.2f;

	glm::vec3 camPos = { 0, 0, 0 };
	glm::vec2 playerInput;
	glm::vec2 rotation = glm::vec2(0, 0);
	glm::vec2 lastMousePos;

	{
		// Initialize mouse position
		double lastMousePosX, lastMousePosY;
		glfwGetCursorPos(app.window, &lastMousePosX, &lastMousePosY);
		lastMousePos = glm::vec2(lastMousePosX, lastMousePosY);
	}

	// Timing
	double lastFrameTime = glfwGetTime();
	LOG("Frame " << lastFrameTime);

	camera.SetProjection(90.0f, app.aspectRatio);

	const glm::vec3 RIGHT = glm::vec3(1, 0, 0);
	const glm::vec3 UP = glm::vec3(0, 1, 0);
	const glm::vec3 FORWARD = glm::vec3(0, 0, 1);

	// Data passed to shader
	ImVec4 color1{ 0.7f, 0.3f, 1.0f, 1.0f };
	ImVec4 color2{ 0.0f, 1.0f, 1.0f, 1.0f };
	float shader_mult = 0.3f;
	float shader_range = 1;

	// objects
	std::vector<Model> objects;
	objects.reserve(10);

	glm::vec3 pos1(1.5f, 0.0f, -5.0f);
	glm::vec3 pos2(-1.5f, 0.0f, -5.0f);

	Model t(pos1, mesh);
	//Model t2({ -3, 0, -10 }, mesh);
	Model t3(pos2, mesh);

	objects.push_back(t);
	//objects.push_back(t2);
	objects.push_back(t3);

	const int monkeys = 80;
	for (size_t y = 0; y < monkeys; y++)
	{
		//InstrumentationTimer timer11("Shoot");
		for (size_t x = 0; x < monkeys; x++)
		{
			Model m({ x * 2, y * 2, -10 }, mesh);
			objects.push_back(m);
		}
	}

	// GAME LOOP
	while (!glfwWindowShouldClose(app.window))
	{
		PROFILE_SCOPE("Game Loop");

		// Time
		const double time = glfwGetTime();
		const float dt = (float)(time - lastFrameTime);
		lastFrameTime = time;

		pos2.y = sin(time * 2) * 2;
		objects[1].SetPosition(pos2);
		objects[1].SetRotation(vec3(time * 3, 0, 0));
		objects[1].SetScale(vec3(2, 2, 2));
		objects[1].SetScale(2);

		// Input
		glfwPollEvents();

		playerInput.x = (float)(KeyPressed(GLFW_KEY_A) ? -1 : KeyPressed(GLFW_KEY_D) ? 1 : 0);
		playerInput.y = (float)(KeyPressed(GLFW_KEY_W) ? -1 : KeyPressed(GLFW_KEY_S) ? 1 : 0);

		double mousePosX, mousePosY;
		glfwGetCursorPos(app.window, &mousePosX, &mousePosY);
		auto mousePos = glm::vec2(mousePosX, mousePosY);

		glm::vec2 mouseDelta = mousePos - lastMousePos;
		lastMousePos = mousePos;

		if (mouseView)
		{
			glm::vec2 delta = mouseDelta * mouseSensitivity * dt;
			rotation += delta;
		}

		const float rad90 = 1.56f;
		rotation.y = glm::clamp(rotation.y, -rad90, rad90);

		// Camera
		camera.SetInputRotation(rotation);

		camera.UpdateRotation();
		camera.MoveRelative(playerInput * dt * cameraSpeed);
		camera.Update();

		// Rendering
		renderer.Clear();

		shader.SetVPMatrix(camera.vp);

		// Draw calls
		{
			PROFILE_SCOPE("Draw");

			for (Model& t : objects)
			{
				shader.SetMMatrix(t.LocalToWorld());
				renderer.DrawMesh(t.mesh);
			}
		}

		// imgui read values
		glm::vec4 inputColor1 = from(color1);
		shader.SetVector("_InputColor1", inputColor1);

		glm::vec4 inputColor2 = from(color2);
		shader.SetVector("_InputColor2", inputColor2);

		shader.SetFloat("_Mult", shader_mult);
		shader.SetFloat("_Range", shader_range);

		// imgui
		bool applyResolution = false;
#ifdef USE_GUI
		{
			PROFILE_SCOPE("ImGui");

			GUI::NewFrame();

			//ImGui::ShowDemoWindow();

			{
				ImGui::Begin("So Pro");

				ImGui::ColorEdit3("color 1", (float*)&color1);
				ImGui::ColorEdit3("color 2", (float*)&color2);
				ImGui::SliderFloat("Mult", &shader_mult, 0, 2);
				ImGui::SliderFloat("Range", &shader_range, 0, 2);

				ImGui::DragFloat3("Yaya", &pos1[0], 0.01f, 0, 1);

				ImGui::Text("Window");
				ImGui::DragInt2("Resolution", &targetResolution[0], 4);
				if (ImGui::Button("Apply"))
				{
					applyResolution = true;
				}

				ImGui::SameLine();
				ImGui::Checkbox("Fullscreen", &app.fullscreen);

				// Analitics
				ImGui::Text("DT: %.3f ms, FPS: %.1f, AVG: %.1f", dt, 1.0f / dt, ImGui::GetIO().Framerate);
				ImGui::Text("Mesh: vertices: %i, indices: %i", mesh.vertexCount, mesh.indexCount);

				ImGui::End();
			}

			GUI::Render();
		}
#endif

		app.SwapBuffers();

		if (applyResolution)
		{
			app.ChangeResolution(targetResolution.x, targetResolution.y);
			RebuildEverything();
		}

		if (quitKeyPressed)
			break;
	}

#ifdef USE_GUI
	GUI::Shutdown();
#endif

	shader.Delete();

	app.Terminate();

	Instrumentor::Instance().endSession();

	return 0;
}