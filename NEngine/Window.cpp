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

int Window::CreateWindow()
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	fullscreenWidth = mode->width;
	fullscreenHeight = mode->height;

	const float screenWidth = windowedWidth;
	const float screenHeight = windowedHeight;

	aspectRatio = screenWidth / screenHeight;

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	if (!fullscreen)
		window = glfwCreateWindow(screenWidth, screenHeight, "NEngine", NULL, NULL);
	else
		window = glfwCreateWindow(mode->width, mode->height, "NEngine", monitor, NULL);

	if (!window)
	{
		glfwTerminate();
		LOG("Creating window failed");
		return -1;
	}

	glfwMakeContextCurrent(window);
}

void Window::ToggleFullscreen()
{
	fullscreen = !fullscreen;

	SetFullscreen(fullscreen);
}

void Window::SetFullscreen(bool b)
{
	fullscreen = b;

	glfwDestroyWindow(window);
	CreateWindow();
}

void Window::ChangeResolution(int newWidth, int newHeight)
{
	glfwDestroyWindow(window);

	windowedWidth = newWidth;
	windowedHeight = newHeight;

	CreateWindow();
}

int Window::Initialize()
{
	if (!glfwInit())
		return -1;

	if (!CreateWindow())
		return -1;

	return 0;
}

void Window::SwapBuffers()
{
	glfwSwapBuffers(window);
}

Window::~Window()
{
}
