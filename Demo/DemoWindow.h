//
// Created by scion on 8/9/2023.
//

#pragma once

#include "Window.h"
#include "MemoryPool.h"
#include "GL/Buffer/VAO.h"
#include "GL/Shader/Shader.h"
#include "Entity/MeshRenderer.h"
#include <gECPP/Entity/Entity.h>

namespace VoxelDemo
{
	class DemoWindow final : public gE::Window
	{
	 public:
		DemoWindow(gl::u16vec2 size, const char* name = "VoxelDemo") : gE::Window(size, name) {};
	 private:
		virtual void OnInit();
		virtual void OnUpdate(float);
		virtual void OnRender(float);
		virtual void OnDestroy();

		gE::MeshRenderer* _testMesh;
		GL::Shader* _testShader;


	};
}
