//
// Created by scion on 9/13/2023.
//

#pragma once

#include <windows.h>
#include <gECPP/Engine/Window/Window.h>
#include <Engine/Component/Behavior.h>
#include <Engine/Entity/Entity.h>

#include "Engine/Component/Physics/CharacterController.h"

#define SENSITIVITY 0.1f
#define SPEED 2.f
#define SPEED_MULTIPLIER 2.f

namespace gE::VoxelDemo
{
	class Movement : public Behavior
	{
	 public:
		explicit Movement(Entity* o, CharacterController& controller) :
			Behavior(o),
			_controller(controller)
		{
		}

		void OnUpdate(float delta) override
		{
			Transform& transform = GetOwner().GetTransform();
			Transform& cameraTransform = _camera->GetTransform();

			const KeyboardState& keyboard = GetWindow().GetKeyboard();
			const MouseState& mouse = GetWindow().GetMouse();

			_rot.y += mouse.GetDelta().x * SENSITIVITY;
			_rot.x += mouse.GetDelta().y * SENSITIVITY;
			_rot.x = std::clamp(_rot.x, -89.9f, 89.9f);

			transform.SetRotation(glm::vec3(0, _rot.y * TO_RAD, 0));
			cameraTransform.SetRotation(glm::vec3(_rot.x * TO_RAD, 0, 0));

			glm::vec3 dir(0.f);
			if(IsPressed(keyboard.GetKey(Key::W))) dir.z -= SPEED;
			if(IsPressed(keyboard.GetKey(Key::S))) dir.z += SPEED;
			if(IsPressed(keyboard.GetKey(Key::A))) dir.x -= SPEED;
			if(IsPressed(keyboard.GetKey(Key::D))) dir.x += SPEED;

			dir = normalize(dir);
			if(glm::isnan(dir.x)) dir = DEFAULT;

			if(IsPressed(keyboard.GetKey(Key::LShift))) dir *= SPEED_MULTIPLIER;
			_controller->SetVelocity(transform->Rotation * dir);
		}

		GET_SET_VALUE(Entity*, FPCamera, _camera);

	 private:
		glm::vec3 _rot = DEFAULT;
		RelativePointer<CharacterController> _controller;
		Entity* _camera;
	};
}
