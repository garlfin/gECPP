#include "Window.h"

#include <Component/Camera/Camera.h>
#include <Core/TickHandler.h>
#include <Core/Converter/PVR.h>
#include <Core/Pipeline/DefaultPipeline.h>
#include <Graphics/API/GL/Timer.h>
#include <IMGUI/imgui.h>
#include <IMGUI/backends/imgui_impl_sdl3.h>
#include <Vendor/GLAD/glad.h>
#include <Vendor/SDL3/SDL.h>

using namespace gE;

#define GE_DEBUG_PERFORMANCE

#ifdef GE_DEBUG_PERFORMANCE
	char WindowTitleBuf[256];
#endif

CONSTEXPR_GLOBAL vec2 BlitVertices[6]
{
	vec2(0.f, 0.f),
	vec2(0.f, 1.f),
	vec2(1.f, 0.f),

	vec2(1.f, 0.f),
	vec2(0.f, 1.f),
	vec2(1.f, 1.f)
};

[[noreturn]] void Terminate()
{
	DEBUGBREAK();
	exit(-1);
}

void gE::OverrideSTDTerminate()
{
	std::set_terminate(Terminate);
}

double SDLGetTime(u64 initTime)
{
	const u64 counter = SDL_GetPerformanceCounter() - initTime;
	return (double) counter / (double) SDL_GetPerformanceFrequency();
}

Window::Window(u16vec2 size, const std::string& name) :
	Cameras(this),
	Transforms(this),
	CullingManager(this),
	Behaviors(this),
	Sounds(this),
    Assets(this),
	_size(size),
	_name(name),
	_mouseState(this)
{
	constexpr u32 flags = SDL_INIT_VIDEO | SDL_INIT_GAMEPAD;

	if(!SDL_WasInit(flags))
		if (!SDL_Init(flags))
			Log::FatalError("Failed to initialize SDL.");

	_monitor = Monitor(SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay()));
	_window = SDL_CreateWindow(_name.c_str(), size.x, size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE);
	if(!_window)
		Log::FatalError("Failed to create Window.");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#ifdef DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, true);
#endif

	_graphicsContext = SDL_GL_CreateContext(_window);
	if(!_graphicsContext)
		Log::FatalError(std::format("Failed to create OpenGL context.\n\n{}", SDL_GetError()));

	if(!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
		Log::FatalError("Failed to load OpenGL functions.");

	PVR::Header iconHeader;
	Array<std::byte> iconData = PVR::ReadRaw("Resource/gE.PVR", iconHeader);

	_icon = SDL_CreateSurfaceFrom(iconHeader.Size.x, iconHeader.Size.y, SDL_PIXELFORMAT_RGBA8888, iconData.Data(), 4 * iconHeader.Size.x);
	SDL_SetWindowIcon(_window, _icon);

#ifdef GE_ENABLE_EDITOR
	Editor = ptr_create<Editor::Editor>(this);
#endif

	Log::Write("Vendor: {}\n", (const char*) glGetString(GL_VENDOR));
	Log::Write("Renderer: {}\n", (const char*) glGetString(GL_RENDERER));
	Log::Write("Version: {}\n", (const char*) glGetString(GL_VERSION));
}

Window::~Window()
{
	SDL_DestroySurface(_icon);
	SDL_GL_DestroyContext((SDL_GLContext) _graphicsContext);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

#ifdef DEBUG
	void DebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		if(severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			return;

		if(severity == GL_DEBUG_SEVERITY_HIGH)
			Log::FatalError(std::format("OpenGL critical error:\n\n{}", message));

		Log::WriteLine(message);
	}
#endif

void Window::PollInputs()
{
	_keyboardState.ClearKeyStates();

	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
#ifdef GE_ENABLE_IMGUI
		ImGui_ImplSDL3_ProcessEvent(&event);
#endif
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			CloseState = CloseFlags::Close;
			break;
		case SDL_EVENT_KEY_DOWN:
		case SDL_EVENT_KEY_UP:
			_keyboardState.ProcessKey(event.key);
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			Resize(Size2D(event.window.data1, event.window.data2));
			break;
		default:
			break;
		}
	}

	_keyboardState.ClearShortcutState();
	_mouseState.Update();
	if(_controller) _controller->Update();
}

bool Window::Run()
{
#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	SDL_GL_SetSwapInterval(0);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_MULTISAMPLE);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	OnInit();

	if(SDL_HasGamepad())
	{
		SDL_JoystickID* ids = SDL_GetGamepads(nullptr);
		_controller = ptr_create<ControllerState>(this, ids[0]);
		SDL_free(ids);
	}

	if(VRManager)
		_controller = ptr_create<VRControllerState>(this);

#ifdef GE_DEBUG_PERFORMANCE
	GL::Timer timer(this);
	auto debugTick = TickHandler(GE_DEBUG_POLL_RATE);
#endif

	_physicsTick = TickHandler(GE_PHYSICS_TARGET_TICKRATE);
	_renderTick = TickHandler(GE_RENDER_TARGET_TICKRATE);
	_viewport = Viewport(_size, DEFAULT);

	const u64 initTime = SDL_GetPerformanceCounter();

	while(!(bool) CloseState)
	{
		_time = SDLGetTime(initTime);

		if(_physicsTick.ShouldTick(_time))
		#ifdef GE_ENABLE_EDITOR
			if(Editor->GetIsRunning())
		#endif
			OnFixedUpdate(_physicsTick.GetDelta());

		if(_renderTick.ShouldTick(SDLGetTime(initTime)))
		{
		#ifdef GE_DEBUG_PERFORMANCE
			double updateDelta = SDLGetTime(initTime);

			updateDelta = _renderTick.GetTime() - updateDelta;

			const bool shouldDebugTick = debugTick.ShouldTick(_renderTick.GetTime());
			if(shouldDebugTick) timer.Start();
		#endif

			PollInputs();

			OnUpdate(_renderTick.GetDelta());
			OnRender(_renderTick.GetDelta());

			SDL_GL_SwapWindow(_window);

		#ifdef GE_DEBUG_PERFORMANCE
			if(shouldDebugTick)
			{
				const float renderTime = timer.End() * MS_TO_S;

				sprintf_s(
					WindowTitleBuf,
					"FPS: %u (%u), UPDATE: %u, FIXEDUPDATE: %u (%u)",
					_renderTick.GetTickRate(),
					(unsigned) ceil(1.0 / renderTime),
					(unsigned) ceil(1.0 / updateDelta),
					_physicsTick.GetTickRate(),
					(unsigned) floor(_physicsTick.GetDelta() * GE_PX_MIN_TICKRATE)
				);

				SDL_SetWindowTitle(_window, WindowTitleBuf);
			}
		#endif
		}
	}

	return CloseState == CloseFlags::Restart;
}

void Window::Resize(Size2D size)
{
	_size = size;
#ifdef GE_ENABLE_IMGUI
	GUI->Resize(size);
#endif
}

void Window::OnInit()
{
	if(GLAD_GL_ARB_bindless_texture)
		ShaderCompilationState.emplace_back("EXT_BINDLESS");
	ShaderCompilationState.emplace_back("ENABLE_TAA");

	PipelineBuffers = ptr_create<DefaultPipeline::Buffers>(this);

	Lights = ptr_create<LightManager>(this);
	Cubemaps = ptr_create<CubemapManager>(this);
	Renderers = ptr_create<RendererManager>(this);
	Physics = ptr_create<PhysicsManager>(this);
	GUI = ptr_create<GUIManager>(this);

	GPU::VAO blitVAO = BlitVAOFormat;
	blitVAO.AddBuffer(GPU::Buffer(6 * sizeof(vec2), (const std::byte*) BlitVertices, sizeof(vec2)));
	blitVAO.Materials[0].Count = 2;

	BlitShader = ptr_create<API::Shader>(this, GPU::Shader("Resource/Shader/blit.vert", "Resource/Shader/blit.frag"));
	BlitVAO = ptr_create<API::VAO>(this, move(blitVAO));
	BlitVAO->Free();

	TAAShader = ptr_create<API::ComputeShader>(this, GPU::ComputeShader("Resource/Shader/PostProcess/taa.comp"));
	TonemapShader = ptr_create<API::ComputeShader>(this, GPU::ComputeShader("Resource/Shader/PostProcess/tonemap.comp"));
	BloomShader = ptr_create<API::ComputeShader>(this, GPU::ComputeShader("Resource/Shader/PostProcess/bloom.comp"));
	HiZShader = ptr_create<API::ComputeShader>(this, GPU::ComputeShader("Resource/Shader/Compute/hiz.comp"));
	DefaultMaterial = ptr_create<Material>(this, ref_create<ForwardShader>(this, GPU::Shader("Resource/Shader/uber.vert", "Resource/Shader/missing.frag")));

	PBRMaterialManager = ptr_create<PBRMaterialBuffers>(this);
}

void Window::Blit(const API::Texture& texture)
{
	BlitShader->Bind();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	texture.Use(0);
	BlitVAO->Draw(0);
}

void Window::OnFixedUpdate(float delta)
{
	Physics->OnEarlyFixedUpdate(delta);

	Behaviors.OnFixedUpdate(delta);
	Physics->OnFixedUpdate(delta);
	Transforms.OnFixedUpdate(delta);

	Behaviors.OnLateFixedUpdate(delta);
}

void Window::OnUpdate(float delta)
{
	Entities.MarkDeletions();

	Physics->OnUpdate(delta);
	Cameras.OnUpdate(delta);
	Behaviors.OnUpdate(delta);
	Sounds.OnUpdate(delta);

	Entities.FinalizeDeletions();
}

void Window::OnRender(float delta)
{
	Behaviors.OnRender(delta, nullptr);

	Lights->OnRender(delta, nullptr);
	Cubemaps->OnRender(delta, nullptr);
	Renderers->OnUpdate(delta); // Will tick animated mesh entities

	if(VRManager) VRManager->OnUpdate();

	Transforms.OnUpdate(delta); // Updates Model Matrices
	Transforms.OnRender(delta, nullptr); // Resets flag

	if(!Editor || Editor->OnRender())
		Cameras.OnRender(delta, nullptr);

	GUI->BeginGUI();
	Behaviors.OnGUI(delta);
#ifdef GE_ENABLE_EDITOR
	Editor->OnGUI();
#endif
	GUI->EndGUI();

	GE_ASSERTM(Cameras.GetCurrentCamera(), "CAMERA SHOULD NOT BE NULL!");

	API::Framebuffer::Reset();

	if(!Editor->GetIsOpen())
	{
		glViewport(0, _size.y - _viewport.Size.y, _viewport.Size.x, _viewport.Size.y);
		Blit(Cameras.GetCurrentCamera()->GetColor());
	}

	glViewport(0, 0, _size.x, _size.y);
	Blit(GUI->GetColor());

	if(VRManager)
		VRManager->OnRender();
	else
		Cameras.GetCurrentCamera()->GetCamera().Resize(_viewport.Size);
}

void Window::InitVoxelReflections(const VoxelCaptureSettings& settings)
{
	VoxelBuffers = ptr_create<VoxelPipeline::Buffers>(this);
	VoxelComputeShader = ptr_create<API::ComputeShader>(this, GPU::ComputeShader("Resource/Shader/Compute/voxel.comp"));
	VoxelSceneCapture = ptr_create<VoxelCapture>(this, settings);
	VoxelSceneCapture->SetName("Voxel Capture");
}

Camera3D* Window::GetReflectionSystem() const
{
	if(VoxelSceneCapture.GetPointer()) return &VoxelSceneCapture->GetCamera();
	if(SDFSceneCapture.GetPointer()) return &SDFSceneCapture->GetCamera();
	return nullptr;
}

Monitor::Monitor(const SDL_DisplayMode* mode) :
	Size(mode->w, mode->h),
	RefreshRate(mode->refresh_rate)
{
}
