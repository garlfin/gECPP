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

#define BRDF_SIZE 512
#define BRDF_GROUP_SIZE 8

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

Window::Window(glm::u16vec2 size, const std::string& name) :
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
	if(!SDL_WasInit(SDL_INIT_VIDEO))
		if (!SDL_Init(SDL_INIT_VIDEO))
			Log::FatalError("Failed to initialize SDL.");

	_monitor = Monitor(SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay()));
	_window = SDL_CreateWindow(_name.c_str(), size.x, size.y, SDL_WINDOW_OPENGL | SDL_WINDOW_HIGH_PIXEL_DENSITY);
	if(!_window)
		Log::FatalError("Failed to create Window.");

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#ifdef DEBUG
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, true);
#endif

	_grapicsContext = SDL_GL_CreateContext(_window);
	if(!_grapicsContext)
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
	SDL_GL_DestroyContext((SDL_GLContext) _grapicsContext);
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

bool Window::Run()
{
#ifdef DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugMessage, nullptr);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

	SDL_GL_SetSwapInterval(0);

	OnInit();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_MULTISAMPLE);

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

#ifdef GE_ENABLE_EDITOR
		if(Editor->GetIsRunning())
#endif
		if(_physicsTick.ShouldTick(_time))
			OnFixedUpdate(_physicsTick.GetDelta());

		if(_renderTick.ShouldTick(SDLGetTime(initTime)))
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
				default:
					break;
				}
			}

			_mouseState.Update();

		#ifdef GE_DEBUG_PERFORMANCE
			double updateDelta = SDLGetTime(initTime);

			updateDelta = _renderTick.GetTime() - updateDelta;

			const bool shouldDebugTick = debugTick.ShouldTick(_renderTick.GetTime());
			if(shouldDebugTick) timer.Start();
		#endif

		#ifdef GE_ENABLE_EDITOR
			if(Editor->GetIsRunning())
		#endif
			OnUpdate(_renderTick.GetDelta());
			OnRender(_renderTick.GetDelta());

			SDL_GL_SwapWindow(_window);

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

				SDL_SetWindowTitle(_window, WindowTitleBuf);
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
	GUI = ptr_create<GUIManager>(this);

	GPU::VAO blitVAOFormat = DEFAULT;
	blitVAOFormat.Counts.BufferCount = 1;
	blitVAOFormat.Buffers[0] = GPU::Buffer<std::byte>(1);

	BlitShader = ptr_create<API::Shader>(this, GPU::Shader("Resource/Shader/blit.vert", "Resource/Shader/blit.frag"));
	BlitVAO = ptr_create<API::VAO>(this, move(blitVAOFormat));

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
		brdfTextureSettings.Size = Size2D(BRDF_SIZE);

		BRDFLookup = ptr_create<API::Texture2D>(this, brdfTextureSettings);

		constexpr glm::uvec2 brdfGroupSize = DIV_CEIL_T(BRDF_SIZE, BRDF_GROUP_SIZE, glm::uvec2);

		brdfShader.Bind();
		BRDFLookup->Bind(0, GL_WRITE_ONLY);
		brdfShader.Dispatch(brdfGroupSize);
	}

	DefaultMaterial = ptr_create<Material>(this, ref_create<ForwardShader>(this, GPU::Shader("Resource/Shader/uber.vert", "Resource/Shader/missing.frag")));
}

void Window::Blit(const API::Texture& texture)
{
	BlitVAO->Bind();
	BlitShader->Bind();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	texture.Use(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
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
	Transforms.OnUpdate(delta);
	Sounds.OnUpdate(delta);

	Entities.FinalizeDeletions();
}

void Window::OnRender(float delta)
{
	Behaviors.OnRender(delta, nullptr);

	Lights->OnRender(delta, nullptr);
	Cubemaps->OnRender(delta, nullptr);
	Cameras.OnRender(delta);
	Transforms.OnRender(delta, nullptr);

	GUI->BeginGUI();
	Behaviors.OnGUI(delta);
#ifdef GE_ENABLE_EDITOR
	Editor->OnGUI();
#endif
	GUI->EndGUI();

	GE_ASSERTM(Cameras.GetCurrentCamera(), "CAMERA SHOULD NOT BE NULL!");

	API::Framebuffer::Reset();

	glViewport(0, _size.y - _viewport.Size.y, _viewport.Size.x, _viewport.Size.y);
	Blit(Cameras.GetCurrentCamera()->GetColor());

	glViewport(0, 0, _size.x, _size.y);
	Blit(GUI->GetColor());

	Cameras.GetCurrentCamera()->GetCamera().Resize(_viewport.Size);
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
