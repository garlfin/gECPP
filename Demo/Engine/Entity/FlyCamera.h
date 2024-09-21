//
// Created by scion on 9/13/2023.
//

#pragma once

#include <Demo/Engine/Component/Movement.h>
#include <Engine/Window.h>
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Renderer/PostProcess/Bloom.h>
#include <Engine/Renderer/PostProcess/Tonemap.h>

namespace gE::VoxelDemo
{
	inline ICameraSettings FlyCameraSettings
	{
		ClipPlanes(0.1, 100),
		DefaultCameraTiming,
	};

	class FlyCamera : public Entity
	{
	 public:
		explicit FlyCamera(Window* window) : Entity(window),
			_camera(this, _target, {{ FlyCameraSettings, window->GetSize() }}, &window->GetCameras()),
			_target(*this, _camera, { &_bloom, &_tonemap }),
			_bloom(_target), _tonemap(_target),
			_movement(this)
		{
		}

		GET(gE::PerspectiveCamera&, Camera, _camera);
		GET(gE::DefaultPipeline::Target2D&, Target, _target);
		GET(GL::Texture2D&, Color, _target.GetColor());
		GET(GL::Texture2D&, Depth, _target.GetDepth());

	 private:
		PerspectiveCamera _camera;

		DefaultPipeline::Target2D _target;
		DefaultPipeline::Bloom _bloom;
		DefaultPipeline::Tonemap _tonemap;

		Movement _movement;
	};
}
