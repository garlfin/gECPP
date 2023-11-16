#pragma once

#include <GL/Math.h>
#include <GL/Buffer/VAO.h>
#include <GL/Shader/Shader.h>
#include <GL/Texture/TextureSlotManager.h>

#include "Engine/Manager.h"
#include "Engine/AssetManager.h"
#include "Engine/Component/Transform.h"
#include "Engine/Component/Camera.h"
#include "WindowState.h"
#include "Engine/Renderer/VoxelPipeline.h"
#include "Engine/Entity/VoxelCapture.h"
#include "Engine/Component/MeshRenderer.h"
#include "Engine/Entity/DirectionalLight.h"
#include "Engine/Component/TransformManager.h"

struct GLFWwindow;
struct GLFWvidmode;

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
		SmartPointer<GL::CubemapData> Cubemap;

		GET(VoxelCapture&, VoxelCapture, VoxelCap);
		GET(DirectionalLight&, Sun, Sun);
		GET(gE::Material&, DefaultMaterial, DefaultMaterial);

		// Managers
		GET(CameraManager&, Cameras, Cameras);
		GET(TransformManager&, Transforms, Transforms);
		GET(ComponentManager<Behavior>&, Behaviors, Behaviors);
		GET(ComponentManager<MeshRenderer>&, Renderers, Renderers);
		GET(GL::TextureSlotManager &, SlotManager, SlotManager);

		// Engine States
		RenderStage Stage;

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
		GL::TextureSlotManager SlotManager;

		SmartPointer<VoxelCapture> VoxelCap;
		SmartPointer<DirectionalLight> Sun;
		SmartPointer<gE::Material> DefaultMaterial;
		SmartPointer<GL::Shader> BlitShader;

	 private:
		GL::TextureSize2D _size;
		const char* _name;
		GLFWwindow* _window;
		Monitor _monitor;
		double _time;
	};
}

