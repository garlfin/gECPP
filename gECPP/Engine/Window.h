#pragma once

#include "Engine/Math/Math.h"
#include <Graphics/Buffer/VAO.h>
#include <Graphics/Shader/Shader.h>

#include "WindowState.h"
#include <Engine/Manager.h>
#include <Engine/AssetManager.h>

#include <Engine/Component/Behavior.h>
#include <Engine/Component/Transform.h>
#include <Engine/Component/MeshRenderer.h>
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Component/Cullable.h>

#include <Engine/Entity/VoxelCapture.h>
#include <Engine/Entity/SDFCapture.h>
#include <Engine/Entity/CubemapCapture.h>
#include <Engine/Entity/Light.h>

#include <Engine/Renderer/VoxelPipeline.h>
#include <Engine/Renderer/SDFPipeline.h>
#include <Engine/Renderer/PostProcess/Bloom.h>
#include <Engine/Renderer/PostProcess/Tonemap.h>

struct GLFWwindow;
struct GLFWvidmode;

#define TAA_GROUP_SIZE 8
#define HIZ_GROUP_SIZE 8
#define VOXEL_TAA_GROUP_SIZE 4

namespace gE
{
	struct Monitor
	{
		explicit Monitor(const GLFWvidmode*);
		Monitor() = default;

		const char* Name = nullptr;
		TextureSize2D Size = TextureSize2D(0);
		u16 RefreshRate = 0;

		~Monitor() { delete[] Name; }
	};

	class Window
	{
	 public:
		explicit Window(glm::u16vec2 size, const char* name = "gE");

		bool Run();
		void Blit(const API::Texture& texture);

		// Entities & Data
		Camera3D* GetReflectionSystem() const;

		GET(gE::VoxelCapture*, VoxelCapture, VoxelSceneCapture.Get());
		GET(gE::SDFCapture*, SDFCapture, SDFSceneCapture.Get());

		GET(gE::Material&, DefaultMaterial, DefaultMaterial);
		GET(API::Texture2D&, BRDFLookupTexture, BRDFLookup);

		// Post Process Data
		GET(API::ComputeShader&, TAAShader, TAAShader);
		GET(API::ComputeShader&, TonemapShader, TonemapShader);
		GET(API::ComputeShader&, BloomShader, BloomShader);
		GET(API::ComputeShader&, VoxelTAAShader, VoxelTAAShader);
		GET(API::ComputeShader&, HiZShader, HiZShader);

		// Managers
		GET(CameraManager&, Cameras, Cameras);
		GET(TransformManager&, Transforms, Transforms);
		GET(ComponentManager<Behavior>&, Behaviors, Behaviors);
		GET(RendererManager&, Renderers, Renderers);
		GET(LightManager&, Lights, Lights);
		GET(CubemapManager&, Cubemaps, Cubemaps);
		GET(CullingManager&, CullingManager, CullingManager);

		// Engine States
		RenderFlags State;

		GET_CONST(TextureSize2D, Size, _size);
		GET_CONST(const Monitor&, Monitor, _monitor);
		GET_CONST(VoxelPipeline::Buffers&, VoxelBuffers, VoxelBuffers);
		GET_CONST(DefaultPipeline::Buffers&, PipelineBuffers, PipelineBuffers);
		GET_CONST(double, Time, _time);

		NODISCARD ALWAYS_INLINE GLFWwindow* GLFWWindow() const { return _window; }

		~Window();

	 protected:
		virtual void OnInit();
		virtual void OnUpdate(float);
		virtual void OnRender(float);
		virtual void OnDestroy() {};

		SmartPointer<DefaultPipeline::Buffers> PipelineBuffers;
		SmartPointer<VoxelPipeline::Buffers> VoxelBuffers;

		SmartPointer<VoxelCapture> VoxelSceneCapture;
		SmartPointer<SDFCapture> SDFSceneCapture;

		CameraManager Cameras;
		TransformManager Transforms;
		LightManager Lights;
		CubemapManager Cubemaps;
		RendererManager Renderers;
		CullingManager CullingManager;

		ComponentManager<Behavior> Behaviors;

		SmartPointer<Material> DefaultMaterial;
		SmartPointer<API::Shader> BlitShader;
		SmartPointer<API::Texture2D> BRDFLookup;
		SmartPointer<API::ComputeShader> TAAShader;
		SmartPointer<API::ComputeShader> TonemapShader;
		SmartPointer<API::ComputeShader> BloomShader;
		SmartPointer<API::ComputeShader> VoxelTAAShader;
		SmartPointer<API::ComputeShader> HiZShader;

	 private:
		TextureSize2D _size;
		const char* _name;
		GLFWwindow* _window;
		Monitor _monitor;
		double _time;
	};
}

