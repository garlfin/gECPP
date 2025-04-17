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
		void Resize() override;

	private:
		Attachment<API::TextureCube, GL_DEPTH_ATTACHMENT> _depth;
	};

	class PointLight : public Light
	{
		REFLECTABLE_PROTO("PLIT", PointLight, Light); // P Litty?!

	public:
		PointLight(Window*, u16 resolution);

		void GetGPULight(GPU::Light&) override;

		GET(CameraCube&, Camera, _camera);
		GET(PointLightTarget&, Target, _target);
		GET(API::TextureCube&, Depth, _target.GetDepth());
		GET_SET(float, Radius, _radius);

	private:
		CameraCube _camera;
		PointLightTarget _target;
		float _radius = 0.1;
	};
}