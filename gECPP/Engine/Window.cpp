#include "Window.h"
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "Engine/Renderer/DefaultPipeline.h"

#include <iostream>
#include "Engine/Component/Camera/Camera.h"

using namespace gE;

#ifdef DEBUG
	char WindowTitleBuf[30];
#endif

#define BRDF_SIZE 512
#define BRDF_GROUP_SIZE 32
#define BRDF_GROUP_COUNT ((BRDF_SIZE) / (BRDF_GROUP_SIZE))

Window::Window(glm::u16vec2 size, const char* name) :
	_size(size), _name(strdup(name)), Lights(this), Cubemaps(this)
{
	if(!glfwInit()) GE_FAIL("Failed to initialize GLFW.");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#ifdef DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_MAXIMIZED, false);

	_window = glfwCreateWindow(size.x, size.y, name, nullptr, nullptr);
	if(!_window) GE_FAIL("Failed to create Window.");

	glfwMakeContextCurrent(_window);

	PVR::Header iconHeader;

	u8* iconData = PVR::Read("Resource/gE.PVR", iconHeader);
	GLFWimage image{(int) iconHeader.Size.x, (int) iconHeader.Size.y, iconData};
	glfwSetWindowIcon(_window, 1, &image);

	delete[] iconData;

	_monitor = Monitor(glfwGetVideoMode(glfwGetPrimaryMonitor()));

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
	glfwSwapInterval(0);

	OnInit();

	_time = glfwGetTime();
	double newTime;
	double delta, frameDelta = 0;

	while(!glfwWindowShouldClose(_window))
	{
		glfwPollEvents();

		newTime = glfwGetTime();
		delta = newTime - _time;
		frameDelta += delta;
		_time = newTime;

		OnUpdate((float) delta);

		if(frameDelta < 1.0 / _monitor.RefreshRate) continue;

	#ifdef DEBUG
		sprintf_s(WindowTitleBuf, "FPS: %u, TICK: %f", (unsigned) std::ceil(1.0 / frameDelta), delta);
		glfwSetWindowTitle(_window, WindowTitleBuf);
	#endif

		frameDelta = 0;
		OnRender((float) frameDelta);
		glfwSwapBuffers(_window);
	}
}

void Window::OnInit()
{
	PipelineBuffers = CreateSmartPointer<DefaultPipeline::Buffers>(this);
	VoxelBuffers = CreateSmartPointer<VoxelPipeline::Buffers>(this);

	BlitShader = CreateSmartPointer<GL::Shader>(this, "Resource/Shader/blit.vert", "Resource/Shader/blit.frag");
	TAAShader = CreateSmartPointer<GL::ComputeShader>(this, "Resource/Shader/taa.comp");

	TonemapEffect = CreateSmartPointer<DefaultPipeline::Tonemap>(this);
	BloomEffect = CreateSmartPointer<DefaultPipeline::Bloom>(this);

	{
		GL::ComputeShader brdfShader(this, "Resource/Shader/brdf.comp");
		GL::TextureSettings<GL::TextureDimension::D2D> brdfSettings
		{
			{ GL_RG16F, GL::WrapMode::Clamp, GL::FilterMode::Linear, 1 },
			glm::u32vec2(BRDF_SIZE)
		};

		BRDFLookup = CreateSmartPointer<GL::Texture2D>(this, brdfSettings);
		BRDFLookup->Bind(0, GL_WRITE_ONLY);
		brdfShader.Bind();
		brdfShader.Dispatch(BRDF_GROUP_COUNT, BRDF_GROUP_COUNT, 1);
	}

	auto defaultShader = CreateReference<GL::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/missing.frag");
	DefaultMaterial = CreateSmartPointer<gE::Material>(this, defaultShader);
}

void Window::Blit(const GL::Texture& texture)
{
	BlitShader->Bind();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	texture.Use(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

Monitor::Monitor(const GLFWvidmode* mode) :
	Name(nullptr),
	Size(mode->width, mode->height),
	RefreshRate(mode->refreshRate)
{
}
