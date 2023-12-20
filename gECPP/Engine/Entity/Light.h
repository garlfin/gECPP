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
	class ILightTarget : public IDepthTarget
	{
	 public:
		virtual void GetGLLight(GL::Light&) = 0;
	};

	class DirectionalLightTarget : public RenderTarget<Camera2D>, public ILightTarget
	{
	 public:
		explicit DirectionalLightTarget(Entity&, OrthographicCamera&);

		GET(GL::Texture2D&, Depth, *_depth);
		GET(OrthographicCamera&, Camera, (OrthographicCamera&) RenderTarget<Camera2D>::GetCamera())

		void Setup(float, Camera*) override;
		void RenderPass(float, Camera*) override;
		void GetGLLight(GL::Light& light) override;

	 private:
		Attachment<GL::Texture2D, GL_DEPTH_ATTACHMENT> _depth;
	};

	class DirectionalLight : public Entity
	{
	 public:
		DirectionalLight(Window*, u16 size, float scale, const glm::quat& = glm::identity<glm::quat>());

		GET(OrthographicCamera&, Camera, _camera);
		GET(DirectionalLightTarget&, Target, _target);
		GET(GL::Texture2D&, Depth, _target.GetDepth());

		GET_CONST(float, Scale, _camera.GetScale().y);

	 private:
		OrthographicCamera _camera;
		DirectionalLightTarget _target;
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
