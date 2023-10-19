//
// Created by scion on 9/13/2023.
//

#pragma once

#include "GL/Buffer/VAO.h"
#include "Engine/Component/Camera.h"
#include "Engine/Window.h"
#include "Demo/Engine/Component/Movement.h"

namespace VoxelDemo
{
	class FlyCam : public gE::Entity
	{
	 public:
		explicit FlyCam(gE::Window* window) : gE::Entity(window),
											  Camera(this,
				{
					gE::CameraSettings2D({ (gE::RenderPass) gE::DefaultPipeline::RenderPass2D }, window->GetSize())
				}),
											  Movement(this)
		{}

		gE::PerspectiveCamera Camera;
		Movement Movement;
	};
}