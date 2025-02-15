//
// Created by scion on 10/17/2024.
//

#pragma once

#include "Light.h"

namespace gE
{
	class PointLightTarget : public RenderTarget<CameraCube>, public IDepthTarget
	{
	public:
		explicit PointLightTarget(Light&, CameraCube&);

		GET(API::TextureCube&, Depth, *_depth);
		GET(Light&, Owner, (Light&) IRenderTarget::GetOwner());
		GET(CameraCube&, Camera, (CameraCube&) RenderTarget<CameraCube>::GetCamera())

		bool Setup(float, Camera* callingCamera) override { return !callingCamera; };
		void RenderPass(float, Camera*) override;

	private:
		Attachment<API::TextureCube, GL_DEPTH_ATTACHMENT> _depth;
	};

	class PointLight : public Light
	{
		REFLECTABLE_TYPE_PROTO(PointLight, "gE::PointLight");

	public:
		PointLight(Window*, u16 resolution);

		void GetGPULight(GPU::Light&) override;

		GET(CameraCube&, Camera, _camera);
		GET(PointLightTarget&, Target, _target);
		GET(API::TextureCube&, Depth, _target.GetDepth());

	private:
		CameraCube _camera;
		PointLightTarget _target;
	};
	inline REFLECTABLE_FACTORY_NO_IMPL(PointLight);
}