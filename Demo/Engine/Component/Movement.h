//
// Created by scion on 9/13/2023.
//

#pragma once

#include <Engine/Window.h>
#include <Engine/Component/Behavior.h>
#include <Engine/Entity/Entity.h>

#include "Engine/Component/Physics/CharacterController.h"
#include "GLFW/glfw3.h"

#define SENSITIVITY 0.1f
#define SPEED 2.f
#define SPEED_MULTIPLIER 2.f

namespace gE::VoxelDemo
{
	class Movement : public Behavior
	{
	 public:
		explicit Movement(Entity* o, CharacterController& controller) : Behavior(o),
			_transform(o->GetTransform()), _window(o->GetWindow().GLFWWindow()),
			_controller(controller)
		{
		}

		void OnUpdate(float delta) override
		{
			Transform& transform = *_transform;
			glm::dvec2 mousePos, mouseDelta;
			glfwGetCursorPos(_window, &mousePos.x, &mousePos.y);
			mouseDelta = _prevCursorPos - mousePos;
			_prevCursorPos = mousePos;

			_rot.y += mouseDelta.x * SENSITIVITY;
			_rot.x += mouseDelta.y * SENSITIVITY;
			_rot.x = std::clamp(_rot.x, -89.9f, 89.9f);

			transform.SetRotation(radians(_rot));

			glm::vec3 dir(0.f);
			if(glfwGetKey(_window, GLFW_KEY_W)) dir.z -= SPEED;
			if(glfwGetKey(_window, GLFW_KEY_S)) dir.z += SPEED;
			if(glfwGetKey(_window, GLFW_KEY_D)) dir.x += SPEED;
			if(glfwGetKey(_window, GLFW_KEY_A)) dir.x -= SPEED;

			dir = normalize(dir);
			if(glm::isnan(dir.x)) dir = DEFAULT;

			const float rotRad = glm::radians(_rot.y);
			const float sinY = glm::sin(rotRad);
			const float cosY = glm::cos(rotRad);

			glm::vec3 rotated;
			rotated = glm::vec3(cosY, 0, -sinY) * dir.x;
			rotated += glm::vec3(sinY, 0, cosY) * dir.z;

			if(glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT)) rotated *= SPEED_MULTIPLIER;
			_controller->SetVelocity(rotated);
		}

	 private:
		RelativePointer<Transform> _transform;
		GLFWwindow* const _window = DEFAULT;
		glm::dvec2 _prevCursorPos = DEFAULT;
		glm::vec3 _rot = DEFAULT;
		RelativePointer<CharacterController> _controller;
	};
}
