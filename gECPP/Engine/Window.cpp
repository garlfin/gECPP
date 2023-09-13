#include "Window.h"
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

using namespace gE;

Window::Window(glm::u16vec2 size, const char* name) :
	_size(size)
{
	if(!glfwInit()) GE_FAIL("Failed to initialize GLFW.");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_window = glfwCreateWindow(size.x, size.y, name, nullptr, nullptr);
	if(!_window) GE_FAIL("Failed to create Window.");

	glfwMakeContextCurrent(_window);

	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) GE_FAIL("Failed to initialize GLAD.");
}

Window::~Window()
{
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void Window::Run()
{
	OnInit();

	double time = glfwGetTime(), newTime, delta;

	while(!glfwWindowShouldClose(_window))
	{
		glfwPollEvents();

		newTime = glfwGetTime();
		delta = newTime - time;
		time = newTime;

		OnUpdate((float) delta);
		OnRender((float) delta);

		glfwSwapBuffers(_window);
	}
}

gE::DefaultPipelineBuffers::DefaultPipelineBuffers(Window* window)
	: Scene(window), Camera(window)
{
	Scene.Bind(GL::BufferTarget::Uniform, 0);
	Camera.Bind(GL::BufferTarget::Uniform, 1);
}