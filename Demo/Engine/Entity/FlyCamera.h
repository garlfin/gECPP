//
// Created by scion on 9/13/2023.
//

#pragma once

#include "GL/Buffer/VAO.h"
#include "Engine/Entity/Camera.h"
#include "Engine/Window.h"
#include "Demo/Engine/Component/Movement.h"

void DefaultRenderPass(gE::Window* window, gE::Camera* cam)
{
	window->_testHandle->Draw(0);
}

namespace VoxelDemo
{
	class FlyCam : public gE::Entity
	{
	 public:
		explicit FlyCam(gE::Window* window) :
			gE::Entity(window),
			Camera(this, gE::CameraSettings{window->GetSize(), {0.1f, 100.f}, DefaultRenderPass},
			degree_cast<AngleType::Radian>(80.f)), _movement(this)
		{
		}

		gE::PerspectiveCamera Camera;
		Movement _movement;
	};
}