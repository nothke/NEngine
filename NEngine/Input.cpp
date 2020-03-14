#include "pch.h"
#include "Input.h"
#include <iostream>
#include <GLFW/glfw3.h>

bool Input::GetKeyDown(int keyCode)
{
	return key == keyCode && action == GLFW_PRESS;
}

bool Input::GetKeyUp(int keyCode)
{
	return key == keyCode && action == GLFW_RELEASE;
}

bool Input::GetKey(int keyCode)
{
	return false;
}

void Input::KeyCallback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	this->key = key;
	this->scancode = scancode;
	this->action = action;
	this->mode = mode;

	//if (key == GLFW_KEY_W && action == GLFW_PRESS)
		//std::cout << "PRESSED!" << std::endl;
}

Input::Input()
{
}


Input::~Input()
{
}
