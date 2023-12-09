//
// Created by scion on 12/4/2023.
//

#pragma once

#include <Engine/Renderer/DefaultPipeline.h>
#include <Engine/Component/Camera/PostProcessEffect.h>
#include <GL/Shader/Shader.h>

#define BLOOM_GROUP_SIZE 32
#define BLOOM_MAX_ITERATIONS 5
#define BLOOM_THRESHOLD 1.f
#define BLOOM_KNEE 0.5f

namespace gE::DefaultPipeline
{
	enum class BloomStage : u8
	{
		PrefilterDownsample,
		Downsample,
		Upsample,
		UpsampleComposite
	};
	class Bloom : public PostProcessEffect<Target2D>
	{
	 public:
		explicit Bloom(Window*);

		void RenderPass(DefaultPipeline::Target2D& t, GL::Texture2D& in, GL::Texture2D& out) override;

	 private:
		GL::ComputeShader _shader;
	};
}