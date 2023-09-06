#pragma once

#include <GL/Math.h>
#include "DefaultPipeline.h"

struct GLFWwindow;

namespace gE
{
	class Window
	{
	 public:
		Window(gl::u16vec2 size, const char* name = "gE");

		void Run();

		inline GLFWwindow* GLFWWindow() const { return _window; }

		~Window();

	 protected:
		virtual void OnInit() = 0;
		virtual void OnUpdate(float) = 0;
		virtual void OnRender(float) = 0;
		virtual void OnDestroy() = 0;

		DefaultPipelineBuffers* PipelineBuffers;

	 private:
		gl::u16vec2 _size;
		const char* _name;
		GLFWwindow* _window;
	};
}

