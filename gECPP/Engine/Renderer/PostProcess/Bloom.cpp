//
// Created by scion on 12/8/2023.
//

#include "Bloom.h"
#include <Engine/Window.h>

#define BLOOM_STAGE_DOWNSAMPLE 1
#define BLOOM_STAGE_UPSAMPLE -1

namespace gE::DefaultPipeline
{
	Bloom::Bloom(Target2D& target) : PostProcessEffect(target)
	{
	}

	Bloom::Bloom(Target2D& target, u8, float t, float k) : Bloom(target)
	{
		Threshold = t;
		Knee = k;
	}

	void Bloom::RenderPass(GL::Texture2D& in, GL::Texture2D& out)
	{
		GL::ComputeShader& shader = GetTarget().GetWindow().GetBloomShader();
		u8 mipCount = glm::min<u8>(in.GetMipCount(), Iterations);
		glm::vec4 settings { Threshold, Knee, Intensity, 0.f }; // Mode/MIP

		shader.Bind();

		// Downsample
		for(u8 i = 1; i < mipCount; i++)
		{
			settings.w = BLOOM_STAGE_DOWNSAMPLE * (i - 1);
			shader.SetUniform(0, in.Use(0));
			shader.SetUniform(1, settings);

			in.Bind(1, GL_WRITE_ONLY, i);

			glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			shader.Dispatch(DIV_CEIL_T(out.GetSize(i), BLOOM_GROUP_SIZE, GL::TextureSize2D));
		}

		for(u8 i = 1; i < mipCount; i++)
		{
			u8 mip = mipCount - i - 1;
			settings.w = BLOOM_STAGE_UPSAMPLE * (mip + 1.f);

			in.Bind(0, GL_READ_WRITE, mip);
			if(mip == 0) out.Bind(1, GL_WRITE_ONLY, 0);
			else in.Bind(1, GL_WRITE_ONLY, mip);

			shader.SetUniform(1, settings);

			glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			shader.Dispatch(DIV_CEIL_T(out.GetSize(mip), BLOOM_GROUP_SIZE, GL::TextureSize2D));
		}
	}
}