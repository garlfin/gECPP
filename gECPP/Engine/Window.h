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
		GET(VoxelCapture &, VoxelCapture, VoxelCap);
		GET(DirectionalLight &, Sun, Sun);
		GET(GL::TextureCube &, Cubemap, Cubemap);
		GET(gE::Material &, DefaultMaterial, DefaultMaterial);
		SET_XVAL(SmartPointer<GL::TextureCube>, Cubemap, Cubemap);

		// Managers
		GET(ComponentManager<Camera> &, Cameras, Cameras);
		GET(TransformManager &, Transforms, Transforms);
		GET(ComponentManager<Behavior> &, Behaviors, Behaviors);
		GET(ComponentManager<MeshRenderer> &, Renderers, Renderers);
		GET(GL::TextureSlotManager &, SlotManager, SlotManager);

		// Engine States
		GET_CONST_VALUE(GL::TextureSize2D, Size, _size);
		GET_SET_VALUE(RenderStage, RenderStage, _renderStage);
		GET_CONST(Monitor &, Monitor, _monitor);
		GET_CONST_VALUE(VoxelPipeline::Buffers &, VoxelBuffers, VoxelBuffers);
		GET_CONST_VALUE(DefaultPipeline::Buffers &, PipelineBuffers, PipelineBuffers);

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
		SmartPointer<GL::TextureCube> Cubemap;
		SmartPointer<gE::Material> DefaultMaterial;

	 private:
		GL::TextureSize2D _size;
		const char* _name;
		GLFWwindow* _window;
		RenderStage _renderStage;
		SmartPointer<GL::Shader> _blitShader;
		Monitor _monitor;
	};
}

