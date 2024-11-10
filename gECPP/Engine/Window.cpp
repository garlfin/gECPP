#include "Window.h"

#include <iostream>
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Renderer/DefaultPipeline.h>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Utility/TickHandler.h"

using namespace gE;

#ifdef DEBUG
	char WindowTitleBuf[256];
#endif

#define BRDF_SIZE 512
#define BRDF_GROUP_SIZE 8
#define US_TO_MS (1.0 / 1000000.f)
#define MS_TO_S (1.0 / 1000.f)

Window::Window(glm::u16vec2 size, const char* name) :
	Cameras(this), Transforms(this),
	CullingManager(this), Behaviors(this),
    _size(size), _name(strdup(name))
{
	if (!glfwInit()) GE_FAIL("Failed to initialize GLFW.");

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
	if ((glm::u16vec2) _monitor.Size != size) primaryMonitor = nullptr;

	_window = glfwCreateWindow(size.x, size.y, name, primaryMonitor, nullptr);
	if (!_window) GE_FAIL("Failed to create Window.");

	glfwMakeContextCurrent(_window);

	PVR::Header iconHeader;
	Array<u8> iconData = PVR::Read("Resource/gE.PVR", iconHeader);

	GLFWimage image{ (int) iconHeader.Size.x, (int) iconHeader.Size.y, iconData.Data() };
	glfwSetWindowIcon(_window, 1, &image);

	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) GE_FAIL("Failed to initialize GLAD.");

	LOG("Vendor: " << glGetString(GL_VENDOR));
	LOG("Renderer: " << glGetString(GL_RENDERER));
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
		if(severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
		std::cout << message << std::endl;
	}
#endif

bool Window::Run()
{
#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	Window::OnInit();
	glfwSwapInterval(0);

	OnInit();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

#ifdef DEBUG
	LOG("INIT TOOK " << glfwGetTime());

	GLuint timer;
	u64 timerResult;

	glCreateQueries(GL_TIME_ELAPSED, 1, &timer);

	auto debugTick = TickHandler(GE_DEBUG_POLL_RATE);
#endif

	_physicsTick = TickHandler(GE_PHYSICS_TARGET_TICKRATE);
	_renderTick = TickHandler(144);

	glfwSetTime(0.0);
	while(!glfwWindowShouldClose(_window))
	{
		glfwPollEvents();

		_time = glfwGetTime();

		if(_physicsTick.ShouldTick(_time))
			OnFixedUpdate(_physicsTick.GetDelta());

		if(_renderTick.ShouldTick(_time))
		{
		#ifdef DEBUG
			double updateDelta = glfwGetTime();
		#endif

			OnUpdate(_renderTick.GetDelta());

		#ifdef DEBUG
			updateDelta = glfwGetTime() - updateDelta;

			const bool shouldDebugTick = debugTick.ShouldTick(_time);
			if(shouldDebugTick) glBeginQuery(GL_TIME_ELAPSED, timer);
		#endif

			OnRender(_renderTick.GetDelta());

		#ifdef DEBUG
			if(shouldDebugTick)
			{
				glEndQuery(GL_TIME_ELAPSED);
				glGetQueryObjectui64v(timer, GL_QUERY_RESULT, &timerResult);

				float ms = timerResult * US_TO_MS * MS_TO_S;

				sprintf_s(
					WindowTitleBuf,
					"FPS: %u (%u), UPDATE: %u, FIXEDUPDATE: %u (%u)",
					(unsigned) ceil(1.0 / _renderTick.GetDelta()),
					(unsigned) ceil(1.0 / ms),
					(unsigned) ceil(1.0 / updateDelta),
					(unsigned) ceil(1.0 / _physicsTick.GetDelta()),
					(unsigned) floor(_physicsTick.GetDelta() * GE_PX_MIN_TICKRATE)
				);

				glfwSetWindowTitle(_window, WindowTitleBuf);
			}
		#endif

			glfwSwapBuffers(_window);
		}
	}

	return false;
}

void Window::OnInit()
{
	if(GLAD_GL_ARB_bindless_texture)
		ShaderCompilationState.emplace_back("EXT_BINDLESS");
	ShaderCompilationState.emplace_back("ENABLE_TAA");

	PipelineBuffers = ptr_create<DefaultPipeline::Buffers>(this);
	VoxelBuffers = ptr_create<VoxelPipeline::Buffers>(this);

	Lights = ptr_create<LightManager>(this);
	Cubemaps = ptr_create<CubemapManager>(this);
	Renderers = ptr_create<RendererManager>(this);
	Physics = ptr_create<PhysicsManager>(this);

	BlitShader = ptr_create<API::Shader>(this, "Resource/Shader/blit.vert", "Resource/Shader/blit.frag");
	TAAShader = ptr_create<API::ComputeShader>(this, "Resource/Shader/PostProcess/taa.comp");
	TonemapShader = ptr_create<API::ComputeShader>(this, "Resource/Shader/PostProcess/tonemap.comp");
	BloomShader = ptr_create<API::ComputeShader>(this, "Resource/Shader/PostProcess/bloom.comp");
	VoxelTAAShader = ptr_create<API::ComputeShader>(this, "Resource/Shader/Compute/voxel.comp");
	HiZShader = ptr_create<API::ComputeShader>(this, "Resource/Shader/Compute/hiz.comp");

	{
		API::ComputeShader brdfShader(this, "Resource/Shader/Compute/brdf.comp");
		GPU::TextureSettings2D brdfSettings
		{
			{ GL_RG16F, GPU::WrapMode::Clamp, GPU::FilterMode::Linear, 1 },
			TextureSize2D(BRDF_SIZE)
		};

		glm::uvec2 brdfGroupSize = DIV_CEIL_T(BRDF_SIZE, BRDF_GROUP_SIZE, glm::uvec2);

		BRDFLookup = ptr_create<API::Texture2D>(this, brdfSettings);
		BRDFLookup->Bind(0, GL_WRITE_ONLY);
		brdfShader.Bind();
		brdfShader.Dispatch(brdfGroupSize);
	}

	auto defaultShader = ref_create<API::Shader>(this, "Resource/Shader/uber.vert", "Resource/Shader/missing.frag");
	DefaultMaterial = ptr_create<Material>(this, defaultShader);
}

void Window::Blit(const API::Texture& texture)
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

void Window::OnFixedUpdate(float delta)
{
	Physics->OnEarlyFixedUpdate(delta);
	Behaviors.OnFixedUpdate(delta);
	Physics->OnFixedUpdate(delta);
	Transforms.OnFixedUpdate(delta);
}

void Window::OnUpdate(float delta)
{
	Entities.MarkDeletions();

	Physics->OnUpdate(delta);
	Cameras.OnUpdate(delta);
	Behaviors.OnUpdate(delta);
	Transforms.OnUpdate(delta);

	Entities.FinalizeDeletions();
}

void Window::OnRender(float delta)
{
	Behaviors.OnRender(delta, nullptr);

	Lights->OnRender(delta, nullptr);
	Cubemaps->OnRender(delta, nullptr);
	Cameras.OnRender(delta);

	GE_ASSERT(Cameras.CurrentCamera, "CAMERA SHOULD NOT BE NULL!");

	API::FrameBuffer::Reset();
	Blit(Cameras.CurrentCamera->GetColor());

	Transforms.OnRender(delta, nullptr);
}

Camera3D* Window::GetReflectionSystem() const
{
	if(VoxelSceneCapture.Get()) return &VoxelSceneCapture->GetCamera();
	if(SDFSceneCapture.Get()) return &SDFSceneCapture->GetCamera();
	return nullptr;
}

Monitor::Monitor(const GLFWvidmode* mode) :
	Size(mode->width, mode->height),
	RefreshRate(mode->refreshRate)
{
}
