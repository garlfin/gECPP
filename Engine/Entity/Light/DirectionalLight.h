//
// Created by scion on 10/17/2024.
//

#pragma once

#include "Light.h"

namespace gE
{
	class DirectionalLightTarget : public RenderTarget<Camera2D>, public DepthTarget<Camera2D>
	{
	public:
		explicit DirectionalLightTarget(Light&, OrthographicCamera&);

		GET(API::Texture2D&, Depth, *_depth);
		GET(Light&, Owner, (Light&) IRenderTarget::GetOwner());
		GET(OrthographicCamera&, Camera, (OrthographicCamera&) RenderTarget<Camera2D>::GetCamera())

		bool Setup(float, Camera*) override;
		void RenderPass(float, Camera*) override;
		void Resize() override;

	private:
		Attachment<API::Texture2D, GL_DEPTH_ATTACHMENT> _depth;
	};

	class DirectionalLight : public Light
	{
		REFLECTABLE_TYPE_PROTO(DirectionalLight); // Die lit reference?!

	public:
		DirectionalLight(Window*, u16 size, float scale, const quat& = identity<quat>());

		void GetGPULight(GPU::Light&) const override;

		GET(OrthographicCamera&, Camera, _camera);
		GET(DirectionalLightTarget&, Target, _target);
		GET(API::Texture2D&, Depth, _target.GetDepth());

		GET_CONST(float, Scale, _camera.GetScale().y);

	private:
		OrthographicCamera _camera;
		DirectionalLightTarget _target;
	};
}