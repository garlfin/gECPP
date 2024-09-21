//
// Created by scion on 9/13/2023.
//

#pragma once

#include <Engine/Window.h>
#include <Engine/Component/Behavior.h>
#include <Engine/Entity/Entity.h>

#include "GLFW/glfw3.h"

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

			_rot.y += mouseDelta.x * 0.1f;
			_rot.x += mouseDelta.y * 0.1f;
			_rot.x = std::clamp(_rot.x, -89.9f, 89.9f);

			_transform.SetRotation() = radians(_rot);

			glm::vec3 dir(0.f);
			if(glfwGetKey(_window, GLFW_KEY_W)) dir.z -= 1;
			if(glfwGetKey(_window, GLFW_KEY_S)) dir.z += 1;
			if(glfwGetKey(_window, GLFW_KEY_D)) dir.x += 1;
			if(glfwGetKey(_window, GLFW_KEY_A)) dir.x -= 1;

			dir = normalize(dir);
			if(!glm::isnan(dir.x))
				_transform.SetPosition() += _transform->Rotation * dir * d;
		}

	 private:
		Transform& _transform;
		GLFWwindow* const _window;
		glm::dvec2 _prevCursorPos {};
		glm::vec3 _rot {};
	};
}
