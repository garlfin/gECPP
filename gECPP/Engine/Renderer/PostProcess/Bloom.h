//
// Created by scion on 12/4/2023.
//

#pragma once

#include <Engine/Component/Camera/PostProcessEffect.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include <Graphics/Shader/Shader.h>

#define BLOOM_GROUP_SIZE 8

namespace gE::DefaultPipeline
{
	struct BloomSettings
	{
		PhysicalCameraSettings* PhysicalCamera;

		float Threshold = 2.f;
		float Knee = 0.7f;
		float Intensity = 1.f;
		u8 Iterations = 7;
	};

	class Bloom final : public PostProcessEffect<API::Texture2D, BloomSettings>
	{
	 public:
		POSTPROCESS_CONSTRUCTOR(Bloom, BloomSettings)

		void RenderPass(API::Texture2D& in, API::Texture2D& out) override;
	};
}