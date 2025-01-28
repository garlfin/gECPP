//
// Created by scion on 12/4/2023.
//

#pragma once

#include <Engine/Component/Camera/PostProcessEffect.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include <Graphics/Shader/Shader.h>

#include "PhysicalCamera.h"

#define TONEMAP_GROUP_SIZE 8

namespace gE::PostProcess
{
	class AutoExposure final : public PostProcessEffect<RenderTarget<Camera2D>, PhysicalCameraSettings>
	{
	public:
		POSTPROCESS_CONSTRUCTOR(AutoExposure);

		NODISCARD bool RenderPass(GL::Texture2D& in, GL::Texture2D& out) override;
	};

	class Tonemap final : public PostProcessEffect<RenderTarget<Camera2D>, PhysicalCameraSettings>
	{
	 public:
		POSTPROCESS_CONSTRUCTOR(Tonemap);

		NODISCARD bool RenderPass(API::Texture2D& in, API::Texture2D& out) override;
	};
}