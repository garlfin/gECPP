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
	class Light;
	class DirectionalLight;

	class LightManager : public Manager<Light>
	{
	 public:
		explicit LightManager(Window* window) : _window(window) {};

		DirectionalLight* Sun = nullptr;

		void OnRender(float delta) override;

	 private:
		Window* _window;
	};

	class Light : public Entity, public IDepthTarget
	{
	 public:
		Light(Window*, Camera&, IDepthTarget&);

		virtual void GetGLLight(GL::Light& light) = 0;

		GET(Camera&, Camera, _camera);

	 private:
		Camera& _camera;
	};

	class DirectionalLightTarget : public RenderTarget<Camera2D>, public IDepthTarget
	{
	 public:
		explicit DirectionalLightTarget(Light&, OrthographicCamera&);

		GET(GL::Texture2D&, Depth, *_depth);
		GET(Light&, Owner, (Light&) IRenderTarget::GetOwner());
		GET(OrthographicCamera&, Camera, (OrthographicCamera&) RenderTarget<Camera2D>::GetCamera())

		void Setup(float, Camera*) override;
		void RenderPass(float, Camera*) override;

	 private:
		Attachment<GL::Texture2D, GL_DEPTH_ATTACHMENT> _depth;
	};

	class DirectionalLight : public Light
	{
	 public:
		DirectionalLight(Window*, u16 size, float scale, const glm::quat& = glm::identity<glm::quat>());

		void GetGLLight(GL::Light&) override;

		GET(OrthographicCamera&, Camera, _camera);
		GET(DirectionalLightTarget&, Target, _target);
		GET(GL::Texture2D&, Depth, _target.GetDepth());

		GET_CONST(float, Scale, _camera.GetScale().y);

	 private:
		OrthographicCamera _camera;
		DirectionalLightTarget _target;
	};
}
