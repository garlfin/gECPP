//
// Created by scion on 8/9/2023.
//

#pragma once

#include "Window.h"
#include "Asset/MemoryPool.h"
#include "GL/Buffer/VAO.h"
#include "GL/Shader/Shader.h"
#include "Entity/MeshRenderer.h"
#include <gECPP/Entity/Entity.h>

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

		GL::VAO* _testMesh;
		GL::Shader* _testShader;
	};
}
