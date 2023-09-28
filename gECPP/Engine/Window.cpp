#include "Window.h"
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "Engine/Renderer/DefaultPipeline.h"

#include <iostream>
#include <Engine/Component/Camera.h>

using namespace gE;

Window::Window(glm::u16vec2 size, const char* name) :
	_size(size)
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
	_blitShader = CreateHandle<GL::Shader>(this, "Resource/Shader/blit.vert", "Resource/Shader/blit.frag");
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

gE::DefaultPipeline::Buffers::Buffers(Window* window)
	: _sceneBuffer(window), _cameraBuffer(window)
{
	_sceneBuffer.Bind(GL::BufferTarget::Uniform, 0);
	_cameraBuffer.Bind(GL::BufferTarget::Uniform, 1);
}

void DefaultPipeline::RenderPass2D(Window* window, Camera* camera)
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	window->RasterShader->Bind();
	window->Mesh->Draw(0);
}

void DefaultPipeline::RenderPass3D(Window*, Camera*) {}
void DefaultPipeline::RenderPassDirectionalShadow(Window*, Camera*) {}

#ifdef DEBUG
// this will only really be used in; not too concerned w/ perf
bool PostProcessPass::CheckRequirements(const Camera& cam) const
{
	const AttachmentSettings& settings = cam.GetSettings().RenderAttachments;

	if(Requirements.Depth != settings.Depth) return false;
	if(Requirements.DepthCopy && !settings.DepthCopy) return false;

	#pragma unroll FRAMEBUFFER_MAX_COLOR_ATTACHMENTS
	for(u8 i = 0; i < FRAMEBUFFER_MAX_COLOR_ATTACHMENTS; i++)
	{
		if(!Requirements.Attachments[i]) continue;
		if(Requirements.Attachments[i] != settings.Attachments[i]) return false;
		if(Requirements.ColorCopy[i] && !settings.ColorCopy[i]) return false;
	}
	return true;
}
#endif
