#pragma once

#include <Component/Behavior.h>
#include <Component/Cullable.h>
#include <Component/Transform.h>
#include <Component/Camera/Camera.h>
#include <Component/MeshRenderer/MeshRenderer.h>
#include <Component/Physics/RigidBody.h>
#include <Core/AssetManager.h>
#include <Core/KeyboardState.h>
#include <Core/MouseState.h>
#include <Core/TickHandler.h>
#include <Core/WindowState.h>
#include <Core/GUI/Editor.h>
#include <Core/GUI/GUI.h>
#include <Core/Math/Math.h>
#include <Core/Pipeline/VoxelPipeline.h>
#include <Entity/CubemapCapture.h>
#include <Entity/SDFCapture.h>
#include <Entity/VoxelCapture.h>
#include <Entity/Light/Light.h>
#include <Graphics/Texture/TextureSlotManager.h>
#include <SDL3/SDL_surface.h>

#define GE_REFRESH_RATE _monitor.RefreshRate

#define GE_DEBUG_POLL_RATE 16
#define GE_UPDATE_TARGET_TICKRATE 60
#define GE_RENDER_TARGET_TICKRATE GE_REFRESH_RATE
#define GE_PHYSICS_TARGET_TICKRATE GE_PX_MIN_TICKRATE

struct SDL_Window;
struct SDL_DisplayMode;

namespace gE
{
	struct Monitor
	{
		explicit Monitor(const SDL_DisplayMode*);
		Monitor() = default;

		const char* Name = DEFAULT;
		Size2D Size = Size2D(0);
		u16 RefreshRate = 0;

		~Monitor() { delete[] Name; }
	};

	void OverrideSTDTerminate();

	class Window
	{
	 public:
		explicit Window(glm::u16vec2 size, const std::string& name = "gE");

		bool Run();

		void Blit(const API::Texture& texture);

		// Entities & Data
		Camera3D* GetReflectionSystem() const;

		GET(gE::VoxelCapture*, VoxelCapture, VoxelSceneCapture.GetPointer());
		GET(gE::SDFCapture*, SDFCapture, SDFSceneCapture.GetPointer());

		GET(gE::Material&, DefaultMaterial, DefaultMaterial);
		GET(API::Texture2D&, BRDFLookupTexture, BRDFLookup);
		GET(std::vector<GPU::PreprocessorPair>&, ShaderCompilationState, ShaderCompilationState);

		// Post Process Data
		GET(API::ComputeShader&, TAAShader, TAAShader);
		GET(API::ComputeShader&, TonemapShader, TonemapShader);
		GET(API::ComputeShader&, BloomShader, BloomShader);
		GET(API::ComputeShader&, VoxelComputeShader, VoxelComputeShader);
		GET(API::ComputeShader&, HiZShader, HiZShader);

		// Managers
		GET(CameraManager&, Cameras, Cameras);
		GET(TransformManager&, Transforms, Transforms);
		GET(ComponentManager<Behavior>&, Behaviors, Behaviors);
		GET(RendererManager&, Renderers, Renderers);
		GET(LightManager&, Lights, Lights);
		GET(CubemapManager&, Cubemaps, Cubemaps);
		GET(CullingManager&, CullingManager, CullingManager);
		GET(GPU::TextureSlotManager&, SlotManager, SlotManager);
		GET(PhysicsManager&, Physics, Physics);
		GET(EntityManager&, Entities, Entities);
		GET(AssetManager&, Assets, AssetManager);

		// Engine States
		RenderFlags RenderState = DEFAULT;
		CloseFlags CloseState = DEFAULT;

		GET_CONST(Size2D, Size, _size);
		GET_SET(Viewport&, Viewport, _viewport);
		GET_CONST(const Monitor&, Monitor, _monitor);
		GET_CONST(VoxelPipeline::Buffers&, VoxelBuffers, VoxelBuffers);
		GET_CONST(DefaultPipeline::Buffers&, PipelineBuffers, PipelineBuffers);

		GET_CONST(double, Time, _time);
		GET_CONST(double, FrameDelta, _renderTick.GetDelta());
		GET_CONST(double, PhysicsDelta, _physicsTick.GetDelta());

		GET_CONST(const TickHandler&, RenderTick, _renderTick);
		GET_CONST(const TickHandler&, PhysicsTick, _physicsTick);

		GET(KeyboardState&, Keyboard, _keyboardState);
		GET(MouseState&, Mouse, _mouseState);

		GET_CONST(SDL_Window*, SDLWindow, _window);

#ifdef DEBUG
		GET(Editor&, Editor, Editor);
#endif

		virtual ~Window();

	 protected:
		virtual void OnInit();
		virtual void OnFixedUpdate(float);
		virtual void OnUpdate(float);
		virtual void OnRender(float);
		virtual void OnDestroy() {};

		Pointer<DefaultPipeline::Buffers> PipelineBuffers;
		Pointer<VoxelPipeline::Buffers> VoxelBuffers;

		Pointer<VoxelCapture> VoxelSceneCapture;
		Pointer<SDFCapture> SDFSceneCapture;

		std::vector<GPU::PreprocessorPair> ShaderCompilationState;

		CameraManager Cameras;
		TransformManager Transforms;
		Pointer<LightManager> Lights;
		Pointer<CubemapManager> Cubemaps;
		Pointer<RendererManager> Renderers;
		Pointer<PhysicsManager> Physics;
		CullingManager CullingManager;
		GPU::TextureSlotManager SlotManager;
		EntityManager Entities;
		Pointer<GUIManager> GUI;
		ComponentManager<Behavior> Behaviors;

#ifdef DEBUG
		Editor Editor;
#endif

		Pointer<Material> DefaultMaterial;
		Pointer<API::Shader> BlitShader;
		Pointer<API::VAO> BlitVAO;
		Pointer<API::Texture2D> BRDFLookup;
		Pointer<API::ComputeShader> TAAShader;
		Pointer<API::ComputeShader> TonemapShader;
		Pointer<API::ComputeShader> BloomShader;
		Pointer<API::ComputeShader> VoxelComputeShader;
		Pointer<API::ComputeShader> HiZShader;

		AssetManager AssetManager;

	 private:
		SDL_Window* _window;
		SDL_Surface* _icon;
		void* _grapicsContext;

		Size2D _size;
		Viewport _viewport;
		std::string _name;

		double _time = DEFAULT;
		TickHandler _renderTick = DEFAULT;
		TickHandler _physicsTick = DEFAULT;

		Monitor _monitor;
		KeyboardState _keyboardState;
		MouseState _mouseState;
	};
}

