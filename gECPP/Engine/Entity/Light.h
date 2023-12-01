//
// Created by scion on 10/23/2023.
//

#pragma once

#include "Entity.h"
#include "Engine/Component/Camera/Camera.h"

namespace gE
{
	class IShadowTarget : public IRenderTarget
	{
	 public:
		IShadowTarget(Camera& c, GL::Texture& d) : IRenderTarget(c), _depth(d) { }

		GET(GL::Texture&, Depth, _depth);

	 private:
		GL::Texture& _depth;
	};

	class DirectionalShadowTarget : public IShadowTarget
	{
	 public:
		DirectionalShadowTarget(Camera&);

		GET(GL::Texture2D&, Depth, _depth);

		void RenderPass() override;

	 private:
		GL::Texture2D _depth;
	};

	class Light : public Entity
	{
	 public:
		Light(Window*, Camera&, Entity* = nullptr);

		GET(Camera&, Camera, _camera);
		GET(GL::Texture&, Depth, _target.GetDepth());

		virtual void GetGLLight(GL::LightData&) = 0;

	 private:
		Camera& _camera;
		IShadowTarget& _target;
	};

	class DirectionalLight : public Light
	{
	 public:
		DirectionalLight(Window*, u16 size, float scale, const glm::quat& = glm::identity<glm::quat>());

		GET(OrthographicCamera&, Camera, _camera);
		GET(DirectionalShadowTarget&, Target, _target);
		GET(GL::Texture2D&, Depth, _target.GetDepth());

		GET_CONST(float, Scale, _camera.GetScale().y);

		void GetGLLight(GL::LightData&) override;
		void OnRender(float delta) override;

	 private:
		OrthographicCamera _camera;
		DirectionalShadowTarget _target;
	};

	class LightManager : public TypedManager<Light>
	{
	 public:
		explicit LightManager(Window* window) : _window(window) {};

		DirectionalLight* Sun = nullptr;

		void OnRender(float delta) override;

	 private:
		Window* _window;
	};
}
