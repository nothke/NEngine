#pragma once
#include <GLFW/glfw3.h>

class Window
{
public:
	GLFWwindow* window;

	bool fullscreen = false;

	int windowedWidth = 800;
	int windowedHeight = 600;

	int fullscreenWidth = 0;
	int fullscreenHeight = 0;

	float aspectRatio;

	void ToggleFullscreen();

	void SetFullscreen(bool b);

	int Initialize();

	void SwapBuffers();

	Window();

	~Window();
};

