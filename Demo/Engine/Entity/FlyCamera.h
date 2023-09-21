//
// Created by scion on 9/13/2023.
//

#pragma once

#include "GL/Buffer/VAO.h"
#include "Engine/Component/Camera.h"
#include "Engine/Window.h"
#include "Demo/Engine/Component/Movement.h"

void DefaultRenderPass(VoxelDemo::DemoWindow* window, gE::PerspectiveCamera<gE::DefaultRenderTarget>* cam)
{
	window->VoxelShader->Bind();
	window->ExportTexture->Bind();
	window->Mesh->Draw(0);
}

namespace VoxelDemo
{
	class FlyCam : public gE::Entity
	{
	 public:
		explicit FlyCam(gE::Window* window) : gE::Entity(window),
			Camera(this, gE::CameraSettings{window->GetSize(), {0.1f, 100.f}, (gE::RenderPass) DefaultRenderPass},
			degree_cast<AngleType::Radian>(80.f)), _movement(this)
		{
		}

		gE::PerspectiveCamera<gE::DefaultRenderTarget> Camera;
		Movement _movement;
	};
}