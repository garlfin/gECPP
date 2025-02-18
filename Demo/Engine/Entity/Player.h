//
// Created by scion on 9/13/2023.
//

#pragma once

#include <Component/Camera/Camera.h>
#include <Component/Physics/CharacterController.h>
#include <Demo/Engine/Component/Movement.h>
#include <gECPP/Window/Window.h>
#include <Renderer/PostProcess/Bloom.h>
#include <Renderer/PostProcess/TAA.h>
#include <Renderer/PostProcess/Tonemap.h>

namespace gE::VoxelDemo
{
	inline ICameraSettings FlyCameraSettings
	{
		ClipPlanes(0.1, 100),
		DEFAULT,
	};

	class Player : public Entity
	{
		REFLECTABLE_PROTO(Player, Entity, "gE::Player");

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
	inline REFLECTABLE_FACTORY_NO_IMPL(Player);

	inline REFLECTABLE_ONGUI_IMPL(Player,
	{
		Editor::DrawField(Field{ "Movement"sv }, _movement, depth);
		Editor::DrawField(Field{ "Character Controller"sv }, _controller, depth);
	});

	class PlayerCamera final : public Entity
	{
		REFLECTABLE_PROTO(PlayerCamera, Entity, "gE::PlayerCamera");

	public:
		PlayerCamera(Window* window, Player& player) : Entity(window, &player),
			_camera(this, _target, {{ FlyCameraSettings, window->GetSize() }}, &window->GetCameras()),
			_target(*this, _camera, { &_taa, &_bloom, &_tonemap }),
			_bloomSettings{ &_physicalCamera },
			_taaSettings{ &DefaultPipeline::ColorFormat, &_target.GetVelocity(), &_target.GetPreviousDepth() },
			_taa(&_target, &_taaSettings),
			_bloom(&_target, &_bloomSettings),
			_tonemap(&_target, &_physicalCamera)
		{
			const auto offset = glm::vec3(0.f, player.GetController().GetShape()->Height / 2.f, 0.f);
			GetTransform().SetPosition(offset);
		}

		GET(gE::PerspectiveCamera&, Camera, _camera);
		GET(gE::DefaultPipeline::Target2D&, Target, _target);
		GET(GL::Texture2D&, Color, _target.GetColor());
		GET(GL::Texture2D&, Depth, _target.GetDepth());

		GET(PostProcess::PhysicalCameraSettings&, PhysicalCameraSettings, _physicalCamera);
		GET(PostProcess::BloomSettings&, BloomSettings, _bloomSettings);

	private:
		PerspectiveCamera _camera;
		DefaultPipeline::Target2D _target;

		PostProcess::PhysicalCameraSettings _physicalCamera = DEFAULT;
		PostProcess::BloomSettings _bloomSettings = DEFAULT;
		PostProcess::TAASettings _taaSettings;

		PostProcess::TAA _taa;
		PostProcess::Bloom _bloom;
		PostProcess::Tonemap _tonemap;
	};
	inline REFLECTABLE_FACTORY_NO_IMPL(PlayerCamera);

	inline REFLECTABLE_ONGUI_IMPL(PlayerCamera,
	{
		Editor::DrawField(Field{ "Camera"sv, ""}, _camera, depth);
	});
}
