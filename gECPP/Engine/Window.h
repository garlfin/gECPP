#pragma once

#include <GL/Math.h>
#include <GL/Buffer/VAO.h>
#include <GL/Shader/Shader.h>
#include <GL/Texture/TextureSlotManager.h>

#include "WindowState.h"
#include <Engine/Manager.h>
#include <Engine/AssetManager.h>
#include <Engine/Component/Transform.h>
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Renderer/VoxelPipeline.h>
#include <Engine/Entity/VoxelCapture.h>
#include <Engine/Component/MeshRenderer.h>
#include <Engine/Entity/Light.h>
#include <Engine/Entity/CubemapCapture.h>
#include <Engine/Renderer/PostProcess/Bloom.h>
#include <Engine/Renderer/PostProcess/Tonemap.h>

struct GLFWwindow;
struct GLFWvidmode;

#define TAA_GROUP_SIZE 32

namespace gE
{
	struct Monitor
	{
		explicit Monitor(const GLFWvidmode*);
		Monitor() = default;

		const char* Name = nullptr;
		GL::TextureSize2D Size = GL::TextureSize2D(0);
		u16 RefreshRate = 0;

		~Monitor() { delete[] Name; }
	};

	class Window
	{
	 public:
		explicit Window(glm::u16vec2 size, const char* name = "gE");

		void Run();
		void Blit(const GL::Texture& texture);

		// Entities & Data
		GET(VoxelCapture&, VoxelCapture, VoxelCap);
		GET(gE::Material&, DefaultMaterial, DefaultMaterial);
		GET(GL::Texture2D&, BRDFLookupTexture, BRDFLookup);

		// Post Process Data
		GET(GL::ComputeShader&, TAAShader, TAAShader);
		GET(GL::ComputeShader&, TonemapShader, TonemapShader);
		GET(GL::ComputeShader&, BloomShader, BloomShader);

		// Managers
		GET(CameraManager&, Cameras, Cameras);
		GET(TransformManager&, Transforms, Transforms);
		GET(ComponentManager<Behavior>&, Behaviors, Behaviors);
		GET(ComponentManager<MeshRenderer>&, Renderers, Renderers);
		GET(LightManager&, Lights, Lights);
		GET(CubemapManager&, Cubemaps, Cubemaps);
		GET(GL::TextureSlotManager&, SlotManager, SlotManager);

		// Engine States
		RenderFlags State;

		GET_CONST(GL::TextureSize2D, Size, _size);
		GET_CONST(const Monitor&, Monitor, _monitor);
		GET_CONST(VoxelPipeline::Buffers&, VoxelBuffers, VoxelBuffers);
		GET_CONST(DefaultPipeline::Buffers&, PipelineBuffers, PipelineBuffers);
		GET_CONST(double, Time, _time);

		NODISCARD ALWAYS_INLINE GLFWwindow* GLFWWindow() const { return _window; }

		~Window();

	 protected:
		virtual void OnInit();
		virtual void OnUpdate(float) = 0;
		virtual void OnRender(float) = 0;
		virtual void OnDestroy() = 0;

		SmartPointer<DefaultPipeline::Buffers> PipelineBuffers;
		SmartPointer<VoxelPipeline::Buffers> VoxelBuffers;

		CameraManager Cameras{};
		TransformManager Transforms;
		ComponentManager<Behavior> Behaviors;
		ComponentManager<MeshRenderer> Renderers;
		ComponentManager<Entity> Entities;
		LightManager Lights;
		CubemapManager Cubemaps;
		GL::TextureSlotManager SlotManager;

		SmartPointer<VoxelCapture> VoxelCap;
		SmartPointer<gE::Material> DefaultMaterial;
		SmartPointer<GL::Shader> BlitShader;
		SmartPointer<GL::Texture2D> BRDFLookup;
		SmartPointer<GL::ComputeShader> TAAShader;
		SmartPointer<GL::ComputeShader> TonemapShader;
		SmartPointer<GL::ComputeShader> BloomShader;

	 private:
		GL::TextureSize2D _size;
		const char* _name;
		GLFWwindow* _window;
		Monitor _monitor;
		double _time;
	};
}

