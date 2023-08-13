//
// Created by scion on 8/9/2023.
//

#pragma once

#include "Window.h"

namespace VoxelDemo
{
	class DemoWindow final : public gECPP::Window
	{
	 public:
		DemoWindow(gl::u16vec2 size, const char* name = "VoxelDemo") : gECPP::Window(size, name) {};
	 private:
		virtual void OnInit();
		virtual void OnUpdate(float);
		virtual void OnRender(float);
	};
}
