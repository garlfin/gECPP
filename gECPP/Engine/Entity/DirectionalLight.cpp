//
// Created by scion on 10/23/2023.
//

#include "DirectionalLight.h"

namespace gE
{
	const SizelessCameraSettings DirectionalSettings
	{
		(RenderPass) DefaultPipeline::RenderPassDirectionalShadow,
		ClipPlanes(0.1, 100),
		DefaultCameraTiming,
		DefaultPipeline::AttachmentShadow
	};

	DirectionalLight::DirectionalLight(Window* w, u16 size, float scale) : Entity(w),
		_camera(this, { { DirectionalSettings, glm::u32vec2(size) }, glm::vec4(scale) })
	{

	}

	void DirectionalLight::OnRender(float delta)
	{
		
	}
}


