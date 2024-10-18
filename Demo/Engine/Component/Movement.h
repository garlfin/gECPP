//
// Created by scion on 9/13/2023.
//

#pragma once

#include <Engine/Window.h>
#include <Engine/Component/Behavior.h>
#include <Engine/Entity/Entity.h>

#include "GLFW/glfw3.h"

#define SENSITIVITY 0.1f
#define SPEED 2.f
#define SPEED_MULTIPLIER 2.f

namespace gE::VoxelDemo
{
	class Movement : public Behavior
	{
	 public:
		explicit Movement(Entity* o) : Behavior(o),
			_transform(o->GetTransform()), _window(o->GetWindow().GLFWWindow())
		{
		}

		void OnUpdate(float d) override
		{
			glm::dvec2 mousePos, mouseDelta;
			glfwGetCursorPos(_window, &mousePos.x, &mousePos.y);
			mouseDelta = _prevCursorPos - mousePos;
			_prevCursorPos = mousePos;

			_rot.y += mouseDelta.x * SENSITIVITY;
			_rot.x += mouseDelta.y * SENSITIVITY;
			_rot.x = std::clamp(_rot.x, -89.9f, 89.9f);

			_transform.SetRotation(radians(_rot));

			glm::vec3 dir(0.f);
			if(glfwGetKey(_window, GLFW_KEY_W)) dir.z -= SPEED;
			if(glfwGetKey(_window, GLFW_KEY_S)) dir.z += SPEED;
			if(glfwGetKey(_window, GLFW_KEY_D)) dir.x += SPEED;
			if(glfwGetKey(_window, GLFW_KEY_A)) dir.x -= SPEED;

			dir = normalize(dir);
			if(glm::isnan(dir.x)) return;

			if(glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT)) dir *= SPEED_MULTIPLIER;
			_transform.SetPosition(_transform->Position + _transform->Rotation * dir * d);
		}

	 private:
		Transform& _transform;
		GLFWwindow* const _window;
		glm::dvec2 _prevCursorPos {};
		glm::vec3 _rot {};
	};
}
