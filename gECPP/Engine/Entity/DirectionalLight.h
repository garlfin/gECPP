//
// Created by scion on 10/23/2023.
//

#pragma once

#include "Entity.h"
#include <Engine/Component/Camera.h>

namespace gE
{
	class Light : public Entity
	{
	 public:
		Light(Window*, Camera&, Entity* = nullptr);

		GET(Camera&, Camera, _camera);
		GET(GL::Texture*, Depth, _camera.GetDepthAttachment());

		virtual void GetGLLight(GL::LightData&) = 0;

	 private:
		Camera& _camera;
	};

	class DirectionalLight : public Light
	{
	 public:
		DirectionalLight(Window*, u16 size, float scale, const glm::quat& = glm::identity<glm::quat>());

		GET(OrthographicCamera&, Camera, _camera);
		GET(GL::Texture2D*, Depth, _camera.GetDepthAttachment());
		GET_CONST(float, Scale, _scale);

		void GetGLLight(GL::LightData&) override;
		void OnRender(float delta) override;

	 private:
		OrthographicCamera _camera;
		float _scale;
	};

	class LightManager : public TypedManager<Light>
	{
	 public:
		LightManager(Window* window) : _window(window) {};
		DirectionalLight* Sun = nullptr;

		void OnRender(float delta) override;

	 private:
		Window* _window;
	};
}
