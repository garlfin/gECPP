//
// Created by scion on 9/13/2023.
//

#pragma once

#include "Engine/Component/Behavior.h"
#include "Engine/Entity/Entity.h"
#include "Engine/Window.h"
#include "GLFW/glfw3.h"

namespace VoxelDemo
{
	class Movement : public gE::Behavior
	{
	 public:
		explicit Movement(gE::Entity* o) : gE::Behavior(o),
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

			_transform.SetRotation(glm::radians(_rot));

			glm::vec3 dir(0.f);
			if(glfwGetKey(_window, GLFW_KEY_W)) dir.z -= 1;
			if(glfwGetKey(_window, GLFW_KEY_S)) dir.z += 1;
			if(glfwGetKey(_window, GLFW_KEY_D)) dir.x += 1;
			if(glfwGetKey(_window, GLFW_KEY_A)) dir.x -= 1;

			dir = glm::normalize(dir);
			if(!glm::isnan(dir.x)) _transform.Position += _transform.Rotation * dir * d;
		}

	 private:
		gE::Transform& _transform;
		GLFWwindow* const _window;
		glm::dvec2 _prevCursorPos {};
		glm::vec3 _rot {};
	};
}