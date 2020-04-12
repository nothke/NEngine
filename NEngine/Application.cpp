#include "Application.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <instrumentor.h>

#define LOG(x) std::cout << x << std::endl

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int Application::CreateWindow()
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	//fullscreenWidth = mode->width;
	//fullscreenHeight = mode->height;

	const int screenWidth = fullscreen ? mode->width : windowedWidth;
	const int screenHeight = fullscreen ? mode->height : windowedHeight;

	aspectRatio = (float)screenWidth / (float)screenHeight;

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(screenWidth, screenHeight, "NEngine", fullscreen ? monitor : NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		LOG("Creating window failed");
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetWindowSize(window, screenWidth * 2, screenHeight * 2);

	// VSync
	glfwSwapInterval(1);

	return 0;
}

void Application::RescaleTo(int width, int height)
{
	glViewport(0, 0, width, height);
}

void Application::ToggleFullscreen()
{
	fullscreen = !fullscreen;

	SetFullscreen(fullscreen);
}

void Application::SetFullscreen(bool b)
{
	fullscreen = b;

	glfwDestroyWindow(window);
	CreateWindow();
}

void Application::ChangeResolution(int newWidth, int newHeight)
{
	glfwDestroyWindow(window);

	windowedWidth = newWidth;
	windowedHeight = newHeight;

	CreateWindow();
}

int Application::Init()
{
	glfwSetErrorCallback(glfw_error_callback);

	if (glfwInit() == GLFW_FALSE)
	{
		LOG("GLFW error");
		return -1;
	}

	if (CreateWindow())
		return -1;

	if (glewInit() != GLEW_OK)
	{
		LOG("GLEW not inited correctly");
		return -1;
	}

	LOG(glGetString(GL_VERSION));

	return 0;
}

void Application::SwapBuffers()
{
	PROFILE_FUNCTION();
	glfwSwapBuffers(window);
}

void Application::Terminate()
{
	glfwTerminate();
}
