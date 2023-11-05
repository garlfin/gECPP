//
// Created by scion on 10/23/2023.
//

#include "CubemapCapture.h"

namespace gE
{
	SizelessCameraSettings CubemapSettings
		{
			(RenderPass) gE::DefaultPipeline::RenderPassCubemap,
			ClipPlanes(0.1, 100),
			CameraTiming(),
			gE::DefaultPipeline::AttachmentsDefault
		};

	CubemapCapture::CubemapCapture(gE::Window* w, u16 size) :
		Entity(w), _camera(this, { CubemapSettings, size })
	{
	}
}
