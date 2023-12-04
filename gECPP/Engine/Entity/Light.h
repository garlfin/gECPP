//
// Created by scion on 10/23/2023.
//

#pragma once

#include "Entity.h"
#include <Engine/Renderer/DefaultPipeline.h>
#include <Engine/Component/Camera/Camera.h>
#include <Engine/Component/Camera/RenderTarget.h>

namespace gE
{
	class DirectionalShadowTarget : public RenderTarget<Camera2D>, public IDepthTarget
	{
	 public:
		explicit DirectionalShadowTarget(OrthographicCamera&);

		GET(GL::Texture2D&, Depth, _depth.Get());
		GET(OrthographicCamera&, Camera, (OrthographicCamera&) RenderTarget<Camera2D>::GetCamera())

		void RenderPass() override;

	 private:
		Attachment<GL::Texture2D, GL_DEPTH_ATTACHMENT> _depth;
	};

	class Light : public Entity
	{
	 public:
		Light(Window*, Camera&, IDepthTarget&, Entity* = nullptr);

		GET(Camera&, Camera, _camera);
		GET(GL::Texture&, Depth, _target.GetDepth());

		virtual void GetGLLight(GL::LightData&) = 0;

	 private:
		Camera& _camera;
		IDepthTarget& _target;
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
