//
// Created by scion on 8/9/2023.
//

#pragma once

#include "Engine/Window.h"
#include "Engine/MemoryPool.h"
#include "GL/Buffer/VAO.h"
#include "GL/Shader/Shader.h"
#include "Engine/Entity/MeshRenderer.h"
#include <Engine/Entity/Entity.h>

namespace VoxelDemo
{
	class DemoWindow final : public gE::Window
	{
	 public:
		DemoWindow(glm::u16vec2 size, const char* name = "VoxelDemo") : gE::Window(size, name) {};

	 private:
		void OnInit() override;
		void OnUpdate(float) override;
		void OnRender(float) override;
		void OnDestroy() override;
	};
}
