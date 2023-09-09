#pragma once

#include <GL/Math.h>
#include "DefaultPipeline.h"
#include <ComponentManager.h>
#include <AssetManager.h>

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
		GET(AssetManager&, Assets, Assets);
		GET(DefaultPipelineBuffers*, PipelineBuffers, PipelineBuffers);
		GET(ComponentManager<Transform>&, Transforms, Transforms);
		GET(ComponentManager<Behavior>&, Behaviors, Behaviors);

		~Window();

	 protected:
		virtual void OnInit() = 0;
		virtual void OnUpdate(float) = 0;
		virtual void OnRender(float) = 0;
		virtual void OnDestroy() = 0;

		DefaultPipelineBuffers* PipelineBuffers;
		AssetManager Assets {};
		ComponentManager<Camera> Cameras {};
		ComponentManager<Transform> Transforms {};
		ComponentManager<Behavior> Behaviors {};

	 private:
		glm::u16vec2 _size;
		const char* _name;
		GLFWwindow* _window;
	};
}

