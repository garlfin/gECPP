#include "Window.h"
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "Engine/Renderer/DefaultPipeline.h"

#include <iostream>
#include <Engine/Component/Camera.h>

using namespace gE;

Window::Window(glm::u16vec2 size, const char* name) :
	_size(size), _name(strdup(name))
{
	if(!glfwInit()) GE_FAIL("Failed to initialize GLFW.");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#ifdef DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	_window = glfwCreateWindow(size.x, size.y, name, nullptr, nullptr);
	if(!_window) GE_FAIL("Failed to create Window.");

	glfwMakeContextCurrent(_window);

	if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) GE_FAIL("Failed to initialize GLAD.");
}

Window::~Window()
{
	delete[] _name;
	glfwDestroyWindow(_window);
	glfwTerminate();
}

#ifdef DEBUG
void DebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::cout << message << std::endl;
}
#endif

void Window::Run()
{
#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	Window::OnInit();
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

void Window::OnInit()
{
	PipelineBuffers = CreateReference<DefaultPipeline::Buffers>(this);
	VoxelBuffers = CreateReference<VoxelPipeline::Buffers>(this);

	_blitShader = CreateReference<GL::Shader>(this, "Resource/Shader/blit.vert", "Resource/Shader/blit.frag");
}

void Window::Blit(const GL::Texture& texture)
{
	_blitShader->Bind();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	texture.Use(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

