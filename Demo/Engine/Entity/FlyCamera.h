//
// Created by scion on 9/13/2023.
//

#pragma once

#include "GL/Buffer/VAO.h"
#include "Engine/Component/Camera/Camera.h"
#include "Engine/Window.h"
#include "Demo/Engine/Component/Movement.h"

namespace VoxelDemo
{
	gE::ICameraSettings FlyCameraSettings
	{
		gE::ClipPlanes(0.1, 100),
		gE::DefaultCameraTiming,
	};

	class FlyCamera : public gE::Entity
	{
	 public:
		explicit FlyCamera(gE::Window* window) : gE::Entity(window),
			_camera(this, &window->GetCameras(), _target, {{ FlyCameraSettings, window->GetSize() }}),
			_target(_camera, { &window->GetBloomEffect(), &window->GetTonemapEffect() }),
			_movement(this)
		{
		}

		GET(gE::PerspectiveCamera&, Camera, _camera);
		GET(gE::DefaultPipeline::Target2D&, Target, _target);
		GET(GL::Texture2D&, Color, _target.GetColor());
		GET(GL::Texture2D&, Depth, _target.GetDepth());

	 private:
		gE::PerspectiveCamera _camera;
		gE::DefaultPipeline::Target2D _target;

		Movement _movement;
	};
}