#pragma once

struct GLFWwindow;

class Application
{
public:
	GLFWwindow* window;

	bool fullscreen = false;

	int currentWidth;
	int currentHeight;

	int windowedWidth = 800;
	int windowedHeight = 600;

	//int fullscreenWidth = 0;
	//int fullscreenHeight = 0;

	float aspectRatio;

	void ToggleFullscreen();
	void SetFullscreen(bool b);
	void ChangeResolution(int newWidth, int newHeight);
	int Init();
	void SwapBuffers();
	void Terminate();

	void RescaleTo(int width, int height);

private:
	int CreateWindow();

};

