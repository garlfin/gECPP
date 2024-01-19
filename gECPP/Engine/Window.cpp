#include "Window.h"
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "Engine/Renderer/DefaultPipeline.h"

#include <iostream>
#include "Engine/Component/Camera/Camera.h"

#define ENABLE_STATISTICS
#define CLAMP_FPS

using namespace gE;

#ifdef ENABLE_STATISTICS
	char WindowTitleBuf[51];
#endif

#define BRDF_SIZE 512
#define BRDF_GROUP_SIZE 8
#define BRDF_GROUP_COUNT ((BRDF_SIZE) / (BRDF_GROUP_SIZE))
#define FPS_POLL_RATE 0.1
#define US_TO_MS 1000000.f

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

	GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
	_monitor = Monitor(glfwGetVideoMode(primaryMonitor));
	if((glm::u16vec2) _monitor.Size != size) primaryMonitor = nullptr;

	_window = glfwCreateWindow(size.x, size.y, name, primaryMonitor, nullptr);
	if(!_window) GE_FAIL("Failed to create Window.");

	glfwMakeContextCurrent(_window);

	PVR::Header iconHeader;
	u8* iconData = PVR::Read("Resource/gE.PVR", iconHeader);

	GLFWimage image{ (int) iconHeader.Size.x, (int) iconHeader.Size.y, iconData };
	glfwSetWindowIcon(_window, 1, &image);

	delete[] iconData;

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

#ifdef ENABLE_STATISTICS
	u64 pollTick = 0;
#endif

	_time = glfwGetTime();

	Window::OnInit();
	glfwSwapInterval(0);

	OnInit();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

#ifdef ENABLE_STATISTICS
	LOG("INIT TOOK " << _time);
#endif

	_time = glfwGetTime();

	double currentTime;
	double delta, frameDelta = 0.0;

	GLuint timer;
	u64 timerResult;

	glCreateQueries(GL_TIME_ELAPSED, 1, &timer);

	while(!glfwWindowShouldClose(_window))
	{
		glfwPollEvents();

		currentTime = glfwGetTime();
		delta = currentTime - _time;
		_time = currentTime;

		OnUpdate((float) delta);

		currentTime = glfwGetTime();
		frameDelta += delta + currentTime - _time;
		_time = currentTime;

	#ifdef CLAMP_FPS
		if(frameDelta < 1.0 / _monitor.RefreshRate) continue;
	#endif

	#ifdef ENABLE_STATISTICS
		bool tickStatistics = _time > FPS_POLL_RATE * pollTick;
		if(tickStatistics) glBeginQuery(GL_TIME_ELAPSED, timer);
	#endif

		OnRender((float) frameDelta);
		glfwSwapBuffers(_window);

	#ifdef ENABLE_STATISTICS
		if(tickStatistics)
		{
			glEndQuery(GL_TIME_ELAPSED);
			glGetQueryObjectui64v(timer, GL_QUERY_RESULT, &timerResult);

			float ms = timerResult / US_TO_MS;

			sprintf_s(WindowTitleBuf, "FPS: %u, TICK: %f, RENDER: %f", (unsigned) std::ceil(1.0 / (ms / 1000)),
				delta * US_TO_MS, ms);
			glfwSetWindowTitle(_window, WindowTitleBuf);
			pollTick++;
		}
	#endif

		frameDelta = 0;
	}
}

void Window::OnInit()
{
	PipelineBuffers = ptr_create<DefaultPipeline::Buffers>(this);
	VoxelBuffers = ptr_create<VoxelPipeline::Buffers>(this);

	BlitShader = ptr_create<GL::Shader>(this, "Resource/Shader/blit.vert", "Resource/Shader/blit.frag");
	TAAShader = ptr_create<GL::ComputeShader>(this, "Resource/Shader/taa.comp");
	TonemapShader = ptr_create<GL::ComputeShader>(this, "Resource/Shader/tonemap.comp");
	BloomShader = ptr_create<GL::ComputeShader>(this, "Resource/Shader/bloom.comp");
	VoxelTAAShader = ptr_create<GL::ComputeShader>(this, "Resource/Shader/voxel.comp");

	{
		GL::ComputeShader brdfShader(this, "Resource/Shader/brdf.comp");
		GL::TextureSettings<GL::TextureDimension::D2D> brdfSettings
		{
			{ GL_RG16F, GL::WrapMode::Clamp, GL::FilterMode::Linear, 1 },
			GL::TextureSize2D(BRDF_SIZE)
		};

		BRDFLookup = ptr_create<GL::Texture2D>(this, brdfSettings);
		BRDFLookup->Bind(0, GL_WRITE_ONLY);
		brdfShader.Bind();
		brdfShader.Dispatch(BRDF_GROUP_COUNT, BRDF_GROUP_COUNT, 1);
	}

	auto defaultShader = ref_create<GL::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/missing.frag");
	DefaultMaterial = ptr_create<gE::Material>(this, defaultShader);
}

void Window::Blit(const GL::Texture& texture)
{
	// This function is really sketchy, vertices are all in the shader.
	// It needs a VAO to be previously bound to work.

	BlitShader->Bind();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	texture.Use(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void Window::OnUpdate(float delta)
{

	Behaviors.OnUpdate(delta);
}

void Window::OnRender(float delta)
{
	Transforms.OnUpdate(delta);
	Behaviors.OnRender(delta);

	Lights.OnRender(delta, nullptr);
	Cubemaps.OnRender(delta, nullptr);

	Cameras.OnRender(delta);

	GE_ASSERT(Cameras.CurrentCamera, "CAMERA SHOULD NOT BE NULL!");

	GL::FrameBuffer::Reset();
	Blit(Cameras.CurrentCamera->GetColor());
}

Monitor::Monitor(const GLFWvidmode* mode) :
	Name(nullptr),
	Size(mode->width, mode->height),
	RefreshRate(mode->refreshRate)
{
}
