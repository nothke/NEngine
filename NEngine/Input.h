#pragma once
#include <GLFW/glfw3.h>

class Input
{
public:
	bool GetKeyDown(int keyCode);
	bool GetKeyUp(int keyCode);
	bool GetKey(int keyCode);
	void KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode);
	Input();
	~Input();

private:
	int key;
	int scancode;
	int action;
	int mode;
};

