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

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int Window::CreateWindow()
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	//fullscreenWidth = mode->width;
	//fullscreenHeight = mode->height;

	const float screenWidth = fullscreen ? mode->width : windowedWidth;
	const float screenHeight = fullscreen ? mode->height : windowedHeight;

	aspectRatio = screenWidth / screenHeight;

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(screenWidth, screenHeight, "NEngine", fullscreen ? monitor : NULL, NULL);

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
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit())
		return -1;

	//glfwSwapInterval(1); // Enable vsync

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
