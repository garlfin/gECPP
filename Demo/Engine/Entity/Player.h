//
// Created by scion on 9/13/2023.
//

#pragma once

#include <Demo/Engine/Component/Movement.h>
#include <gECPP/Engine/Window/Window.h>
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Renderer/PostProcess/Bloom.h>
#include <Engine/Renderer/PostProcess/Tonemap.h>
#include <Engine/Component/Physics/CharacterController.h>

namespace gE::VoxelDemo
{
	inline ICameraSettings FlyCameraSettings
	{
		ClipPlanes(0.1, 100),
		DefaultCameraTiming,
	};

	class Player : public Entity
	{
	 public:
		explicit Player(Window* window) : Entity(window),
			_movement(this, _controller),
			_controller(this, Physics::CapsuleShape())
		{
		}

		GET(Movement&, Movement, _movement);
		GET(CharacterController&, Controller, _controller);

	 private:
		Movement _movement;
		CharacterController _controller;
	};

	class PlayerCamera : public Entity
	{
	public:
		PlayerCamera(Window* window, Player& player) : Entity(window, &player),
			_camera(this, _target, {{ FlyCameraSettings, window->GetSize() }}, &window->GetCameras()),
			_target(*this, _camera, { &_bloom, &_tonemap }),
			_bloom(_target), _tonemap(_target)
		{
			const auto offset = glm::vec3(0.f, player.GetController().GetShape()->Height / 2.f, 0.f);
			GetTransform().SetPosition(offset);
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
	};
}
