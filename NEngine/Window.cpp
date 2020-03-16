#include "Window.h"
#include <iostream>
//#ifndef __glew_h__ // FUCK THIS
//#include <GL/glew.h>
//#endif
#include <GLFW/glfw3.h>

#define LOG(x) std::cout << x << std::endl

Window::Window()
{
}

void Window::ToggleFullscreen()
{
	fullscreen = !fullscreen;

	SetFullscreen(fullscreen);
}

void Window::SetFullscreen(bool b)
{

	fullscreen = b;

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	if (fullscreen)
	{
		glfwSetWindowMonitor(window, monitor,
			0, 0, fullscreenWidth, fullscreenHeight, mode->refreshRate);
	}
	else
	{
		glfwSetWindowMonitor(window, NULL,
			100, 100, windowedWidth, windowedHeight, mode->refreshRate);
	}
}

int Window::Initialize()
{
	if (!glfwInit())
		return -1;

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	fullscreenWidth = mode->width;
	fullscreenHeight = mode->height;

	const float screenWidth = 800;
	const float screenHeight = 600;

	aspectRatio = screenWidth / screenHeight;

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Windowed
	window = glfwCreateWindow(screenWidth, screenHeight, "NEngine", NULL, NULL);

	// Fullscreen
	//window = glfwCreateWindow(screenWidth, screenHeight, "NEngine", monitor, NULL);

	if (!window)
	{
		glfwTerminate();
		LOG("Creating window failed");
		return -1;
	}

	glfwMakeContextCurrent(window);

	return 0;
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(window);
}

Window::~Window()
{
}