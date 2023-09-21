//
// Created by scion on 9/21/2023.
//

#pragma once

#include <GL/Math.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include "Engine/Array.h"

namespace gE
{
	typedef glm::vec2 ClipPlanes;

	struct CameraSettings
	{
		GL::TextureSize2D Size = {1280, 720};
		ClipPlanes ClipPlanes = {0.1, 1000};
		const RenderTarget& RenderPass = DefaultPipeline::RenderTarget2D;
		const Array<PostProcessPass>* PostProcess = nullptr;
	};

	struct PerspectiveCameraSettings : public CameraSettings
	{
		float FOV = degree_cast<AngleType::Radian>(80.f);
	};
}