#pragma once

#include <GL/GLMath.h>

#define GLFW_INIT_FAILURE 1
#define GLFW_WINDOW_FAILURE 2
#define GLAD_LOAD_FAILURE 3

struct GLFWwindow;

namespace gE
{
	class Window
	{
	 public:
		Window(gl::u16vec2 size, const char* name = "gE");

		inline GLFWwindow* GLFWWindow() const { return _window; }

		void Run();

		~Window();

	 private:
		virtual void OnInit() = 0;
		virtual void OnUpdate(float) = 0;
		virtual void OnRender(float) = 0;

		gl::u16vec2 _size;
		const char* _name;
		GLFWwindow* _window;
	};
}

