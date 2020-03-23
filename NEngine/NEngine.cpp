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
#include <random>

#include "Application.h"
#include "AssetManager.h"
#include "Renderer.h"
#include "Camera.h"
#include "Mesh.h"
#include "Conversion.h"
#include "Log.h"
#include "Model.h"
//#define PROFILING
#include "instrumentor.h"
#include "GUI.h"
#include "FrustumCull.h"
#include "Texture.h"

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

Shader* mainShader; // not like this with multiple shaders

AssetManager assets(3, 3, 3);

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

	assets.RebuildAll();
	mainShader = &assets.shaders[0];
	mainShader->Bind();

	InitInputCallbacks();

	camera.SetProjection(90.0f, app.aspectRatio);

#ifdef USE_GUI
	GUI::Shutdown();

	GUI::Init(app.window);
#endif

	LOG("Rebuilt everything after resolution change");
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

	//app.fullscreen = true;
	if (app.Init()) return -1;

#ifdef USE_GUI
	GUI::Init(app.window);
#endif

	// Meshes
	Mesh plainMesh = assets.CreateMesh("../NEngine/res/models/plain.ply");
	Mesh monkeyMesh = assets.CreateMesh("../NEngine/res/models/suza.ply");
	Mesh grassMesh = assets.CreateMesh("../NEngine/res/models/grasso.ply");

	// Shaders
	mainShader = &assets.CreateShader("../NEngine/res/texture.glsl");
	mainShader->Bind();

	// Textures
	Texture grass3DTex = assets.CreateTexture("../NEngine/res/models/grasso.png"); // Texture::Filtering::Nearest, Texture::EdgeMode::Wrap
	Texture grassPlainTex = assets.CreateTexture("../NEngine/res/models/grass.png"); // Texture::Filtering::Nearest, Texture::EdgeMode::Wrap

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

	// Camera
	camera.SetProjection(90.0f, app.aspectRatio);

	const glm::vec3 RIGHT = glm::vec3(1, 0, 0);
	const glm::vec3 UP = glm::vec3(0, 1, 0);
	const glm::vec3 FORWARD = glm::vec3(0, 0, 1);

	// Data passed to shader
	ImVec4 color1{ 200.0f / 255, 164.0f / 255, 100.0f / 255, 1.0f, };
	ImVec4 color2{ 0.0f, 1.0f, 1.0f, 1.0f };
	float shader_mult = 0.3f;
	float shader_range = 1;
	vec4 shader_FogParams = vec4(80.0f, 0, 4.0f, 10.0f);

	// SCENE

	std::vector<Model> objects;
	objects.reserve(10);

	// Plain
	Model t({ 0,0,0 }, plainMesh, grassPlainTex);
	objects.push_back(t);

	/*
	const int monkeys = 10;
	for (size_t y = 0; y < monkeys; y++)
	{
		for (size_t x = 0; x < monkeys; x++)
		{
			Model m({ x * 2, y * 2, -10 }, monkeyMesh);
			objects.push_back(m);
		}
	}*/

	const int grassCount = 80;
	double PI = glm::pi<double>();
	for (size_t y = 0; y < grassCount; y++)
	{
		for (size_t x = 0; x < grassCount; x++)
		{
			Model m({ x,0,y }, grassMesh, grass3DTex);
			float a = (((double)rand() / (RAND_MAX)) + 1) * PI;
			m.SetRotation(vec3(0, a, 0));
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
			glm::vec2 delta = mouseDelta * mouseSensitivity * (1.0f / 60.0f);
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
		renderer.Clear(from(color1));

		Shader shader = *mainShader;

		shader.SetVPMatrix(camera.vp);
		shader.SetVector("_CamPos", vec4(camera.position, 1));
		Frustum frustum = Frustum(camera.vp);

		// Draw calls
		{
			PROFILE_SCOPE("Draw");
			Mesh* meshPtr = nullptr;
			Texture* texPtr = nullptr;

			for (Model& go : objects)
			{
				if (!go.IsVisible(frustum))
					continue;

				if (meshPtr != &go.mesh)
				{
					go.mesh.Bind();
					meshPtr = &go.mesh;
				}

				if (texPtr != go.texture)
				{
					if (go.texture != nullptr)
					{
						go.texture->Bind(0);
						shader.SetInt("_Texture", 0);
						shader.Bind();
					}
					else
						texPtr->Unbind();

					texPtr = go.texture;
				}

				shader.SetMMatrix(go.LocalToWorld());
				renderer.DrawMesh(go.mesh);
			}
		}

		// imgui read values
		glm::vec4 inputColor1 = from(color1);
		shader.SetVector("_InputColor1", inputColor1);

		glm::vec4 inputColor2 = from(color2);
		shader.SetVector("_InputColor2", inputColor2);

		shader.SetFloat("_Mult", shader_mult);
		shader.SetFloat("_Range", shader_range);
		shader.SetVector("_FogParams", shader_FogParams);

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

				ImGui::Text("Fog");
				ImGui::SliderFloat("Fog range", &shader_FogParams.x, 0, 1000, "%.3f", 2.0f);
				ImGui::SliderFloat("Fog power", &shader_FogParams.y, 0, 1);
				ImGui::SliderFloat("Fog height offset", &shader_FogParams.z, -10, 10);
				ImGui::SliderFloat("Fog height mult", &shader_FogParams.w, 0, 10);

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

	assets.Dispose();

	app.Terminate();

	Instrumentor::Instance().endSession();

	return 0;
}