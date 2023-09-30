#pragma once

#include "GL/Math.h"

#include "Engine/ComponentManager.h"
#include "Engine/AssetManager.h"

#include "GL/Buffer/VAO.h"
#include "GL/Shader/Shader.h"

#include "Engine/Component/Transform.h"
#include "Engine/Component/Camera.h"
#include "WindowState.h"

struct GLFWwindow;

#define LOG(MSG) std::cout << MSG << std::endl

namespace gE
{
	class Window
	{
	 public:
		explicit Window(glm::u16vec2 size, const char* name = "gE");

		void Run();

		NODISCARD ALWAYS_INLINE GLFWwindow* GLFWWindow() const { return _window; }
		GET(ComponentManager<Camera>&, Cameras, Cameras);
		GET(DefaultPipeline::Buffers*, PipelineBuffers, PipelineBuffers);
		GET(ComponentManager<Transform>&, Transforms, Transforms);
		GET(ComponentManager<Component>&, Behaviors, Behaviors);
		GET_CONST_VALUE(GL::TextureSize2D, Size, _size);
		GET_SET_VALUE(RenderState, RenderState, _renderState);

		void Blit(const GL::Texture& texture);

		gE::Handle<GL::VAO> Mesh;
		gE::Handle<GL::Texture3D> ExportTexture;
		gE::Handle<GL::Shader> RasterShader, VoxelShader;

		~Window();

	 protected:
		virtual void OnInit();
		virtual void OnUpdate(float) = 0;
		virtual void OnRender(float) = 0;
		virtual void OnDestroy() = 0;

		DefaultPipeline::Buffers* PipelineBuffers;
		CameraManager Cameras {};
		ComponentManager<Transform> Transforms {};
		ComponentManager<Component> Behaviors {};

	 private:
		GL::TextureSize2D _size;
		const char* _name;
		GLFWwindow* _window;
		RenderState _renderState;

		Handle<GL::Shader> _blitShader;
	};
}

