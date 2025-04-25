//
// Created by scion on 12/8/2023.
//

#include "Bloom.h"

#include <Window.h>

#define BLOOM_STAGE_DOWNSAMPLE 1
#define BLOOM_STAGE_UPSAMPLE (-1)

namespace gE::PostProcess
{
	bool Bloom::RenderPass(API::Texture2D& in, API::Texture2D& out)
	{
		const API::ComputeShader& shader = GetWindow().GetBloomShader();

		u8 mipCount = min<u8>(in.GetMipCount(), GetSettings().Iterations);
		vec4 settings { GetSettings().Threshold, GetSettings().Knee, GetSettings().Intensity, 0.f }; // Mode/MIP

		shader.Bind();
		shader.SetUniform(2, GetSettings().PhysicalCamera->Exposure);

		// Downsample
		for(u8 i = 1; i < mipCount; i++)
		{
			settings.w = BLOOM_STAGE_DOWNSAMPLE * (i - 1);
			shader.SetUniform(0, in.Use(0));
			shader.SetUniform(1, settings);

			in.Bind(1, GL_WRITE_ONLY, i);

			glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			shader.Dispatch(DIV_CEIL_T(out.GetSize(i), BLOOM_GROUP_SIZE, Size2D));
		}

		for(u8 i = 1; i < mipCount; i++)
		{
			u8 mip = mipCount - i - 1;
			settings.w = BLOOM_STAGE_UPSAMPLE * (mip + 1.f);

			in.Bind(0, GL_READ_WRITE, mip);
			if(mip) in.Bind(1, GL_WRITE_ONLY, mip);
			else out.Bind(1, GL_WRITE_ONLY, 0);

			shader.SetUniform(1, settings);

			glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			shader.Dispatch(DIV_CEIL_T(out.GetSize(mip), BLOOM_GROUP_SIZE, Size2D));
		}

		return true;
	}
}
