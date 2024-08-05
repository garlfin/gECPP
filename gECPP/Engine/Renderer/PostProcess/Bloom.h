//
// Created by scion on 12/4/2023.
//

#pragma once

#include <Engine/Renderer/DefaultPipeline.h>
#include <Engine/Component/Camera/PostProcessEffect.h>
#include <Graphics/Shader/Shader.h>

#define BLOOM_GROUP_SIZE 8

namespace gE::DefaultPipeline
{
	class Bloom : public PostProcessEffect<Target2D>
	{
	 public:
		explicit Bloom(Target2D&);
		explicit Bloom(Target2D&, u8, float t, float k);

		float Threshold = 2.f;
		float Knee = 0.7f;
		float Intensity = 1.f;

		u8 Iterations = 7;

		void RenderPass(GL::Texture2D& in, GL::Texture2D& out) override;
	};
}