//
// Created by scion on 8/9/2023.
//

#pragma once

#include "Window.h"
#include "MemoryPool.h"
#include <gECPP/Entity/Entity.h>

namespace VoxelDemo
{
	class DemoWindow final : public gE::Window
	{
	 public:
		DemoWindow(gl::u16vec2 size, const char* name = "VoxelDemo") : gE::Window(size, name), _pool() {};
	 private:
		virtual void OnInit();
		virtual void OnUpdate(float);
		virtual void OnRender(float);

		MemoryPool<gE::Entity, 32> _pool;
	};
}
