#include "Window.h"

#include <iostream>
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Renderer/DefaultPipeline.h>

#include <Vendor/GLAD/glad.h>
#include <Vendor/GLFW/glfw3.h>

#include <Engine/Utility/TickHandler.h>

#include "Graphics/API/GL/Timer.h"

using namespace gE;

#define GE_DEBUG_PERFORMANCE

#ifdef GE_DEBUG_PERFORMANCE
	char WindowTitleBuf[256];
#endif

#define BRDF_SIZE 512
#define BRDF_GROUP_SIZE 8

void Terminate()
{
	DEBUGBREAK();
	exit(-1);
}

void gE::OverrideSTDTerminate()
{
	std::set_terminate(Terminate);
}

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

#ifdef GE_DEBUG_PERFORMANCE
	LOG("INIT TOOK " << glfwGetTime());

	GL::Timer timer(this);

	auto debugTick = TickHandler(GE_DEBUG_POLL_RATE);
#endif

	_physicsTick = TickHandler(GE_PHYSICS_TARGET_TICKRATE);
	_renderTick = TickHandler(GE_RENDER_TARGET_TICKRATE);

	glfwSetTime(0.0);
	while(!glfwWindowShouldClose(_window) && !(bool) CloseState)
	{
		glfwPollEvents();

		_time = glfwGetTime();

		if(_physicsTick.ShouldTick(_time))
			OnFixedUpdate(_physicsTick.GetDelta());

		if(_renderTick.ShouldTick(glfwGetTime()))
		{
		#ifdef GE_DEBUG_PERFORMANCE
			double updateDelta = glfwGetTime();

			updateDelta = _renderTick.GetTime() - updateDelta;

			const bool shouldDebugTick = debugTick.ShouldTick(_renderTick.GetTime());
			if(shouldDebugTick) timer.Start();
		#endif

			_keyboardState.Update(_window);
			_mouseState.Update(_window);

			OnUpdate(_renderTick.GetDelta());

			OnRender(_renderTick.GetDelta());

			glfwSwapBuffers(_window);

		#ifdef GE_DEBUG_PERFORMANCE
			if(shouldDebugTick)
			{
				float renderTime = timer.End() * MS_TO_S;

				sprintf_s(
					WindowTitleBuf,
					"FPS: %u (%u), UPDATE: %u, FIXEDUPDATE: %u (%u)",
					_renderTick.GetTickRate(),
					(unsigned) ceil(1.0 / renderTime),
					(unsigned) ceil(1.0 / updateDelta),
					_physicsTick.GetTickRate(),
					(unsigned) floor(_physicsTick.GetDelta() * GE_PX_MIN_TICKRATE)
				);

				glfwSetWindowTitle(_window, WindowTitleBuf);
			}
		#endif
		}
	}

	return CloseState == CloseFlags::Restart;
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

	BlitShader = ptr_create<API::Shader>(this, GPU::Shader("Resource/Shader/blit.vert", "Resource/Shader/blit.frag"));
	TAAShader = ptr_create<API::ComputeShader>(this, GPU::ComputeShader("Resource/Shader/PostProcess/taa.comp"));
	TonemapShader = ptr_create<API::ComputeShader>(this, GPU::ComputeShader("Resource/Shader/PostProcess/tonemap.comp"));
	BloomShader = ptr_create<API::ComputeShader>(this, GPU::ComputeShader("Resource/Shader/PostProcess/bloom.comp"));
	VoxelComputeShader = ptr_create<API::ComputeShader>(this, GPU::ComputeShader("Resource/Shader/Compute/voxel.comp"));
	HiZShader = ptr_create<API::ComputeShader>(this, GPU::ComputeShader("Resource/Shader/Compute/hiz.comp"));

	{
		API::ComputeShader brdfShader(this, GPU::ComputeShader("Resource/Shader/Compute/brdf.comp"));

		GPU::Texture2D brdfTextureSettings;
		brdfTextureSettings.Format = GL_RG16F;
		brdfTextureSettings.WrapMode = GPU::WrapMode::Clamp;
		brdfTextureSettings.Size = TextureSize2D(BRDF_SIZE);

		BRDFLookup = ptr_create<API::Texture2D>(this, brdfTextureSettings);

		constexpr glm::uvec2 brdfGroupSize = DIV_CEIL_T(BRDF_SIZE, BRDF_GROUP_SIZE, glm::uvec2);

		brdfShader.Bind();
		BRDFLookup->Bind(0, GL_WRITE_ONLY);
		brdfShader.Dispatch(brdfGroupSize);
	}

	auto defaultShader = ref_create<ForwardShader>(*this, GPU::Shader("Resource/Shader/uber.vert", "Resource/Shader/missing.frag"));
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

	GE_ASSERTM(Cameras.CurrentCamera, "CAMERA SHOULD NOT BE NULL!");

	API::FrameBuffer::Reset();
	Blit(Cameras.CurrentCamera->GetColor());

	Transforms.OnRender(delta, nullptr);
}

Camera3D* Window::GetReflectionSystem() const
{
	if(VoxelSceneCapture.GetPointer()) return &VoxelSceneCapture->GetCamera();
	if(SDFSceneCapture.GetPointer()) return &SDFSceneCapture->GetCamera();
	return nullptr;
}

Monitor::Monitor(const GLFWvidmode* mode) :
	Size(mode->width, mode->height),
	RefreshRate(mode->refreshRate)
{
}
