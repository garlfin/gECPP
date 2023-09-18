#pragma once

#include "GL/Math.h"
#include "Renderer/DefaultPipeline.h"
#include "Engine/ComponentManager.h"
#include "Engine/AssetManager.h"
#include "GL/Buffer/VAO.h"

struct GLFWwindow;

namespace gE
{
	class Window
	{
	 public:
		explicit Window(glm::u16vec2 size, const char* name = "gE");

		void Run();

		NODISCARD ALWAYS_INLINE GLFWwindow* GLFWWindow() const { return _window; }
		GET(ComponentManager<Camera>&, Cameras, Cameras);
		GET(DefaultPipelineBuffers*, PipelineBuffers, PipelineBuffers);
		GET(ComponentManager<Transform>&, Transforms, Transforms);
		GET(ComponentManager<Behavior>&, Behaviors, Behaviors);
		GET_CONST(glm::TextureSize2D, Size, _size);
		Handle<GL::VAO> _testHandle;
		~Window();

	 protected:
		virtual void OnInit() = 0;
		virtual void OnUpdate(float) = 0;
		virtual void OnRender(float) = 0;
		virtual void OnDestroy() = 0;

		DefaultPipelineBuffers* PipelineBuffers;
		ComponentManager<Camera> Cameras {};
		ComponentManager<Transform> Transforms {};
		ComponentManager<Behavior> Behaviors {};

	 private:
		glm::TextureSize2D _size;
		const char* _name;
		GLFWwindow* _window;
	};
}

