#include "Window.h"
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace gECPP;

Window::Window(gl::u16vec2 size, const char* name) :
	_size(size)
{
	if(!glfwInit()) exit(GLFW_INIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_window = glfwCreateWindow(size.x, size.y, name, nullptr, nullptr);
	if(!_window) exit(GLFW_WINDOW_FAILURE);

	glfwMakeContextCurrent(_window);

	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) exit(GLAD_LOAD_FAILURE);
}

Window::~Window()
{
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void Window::Run()
{
	OnInit();

	while(!glfwWindowShouldClose(_window))
	{
		glfwPollEvents();

		OnUpdate(0);
		OnRender(0);

		glfwSwapBuffers(_window);
	}
}

