//
// Created by scion on 12/8/2023.
//

#include "Bloom.h"

namespace gE::DefaultPipeline
{
	Bloom::Bloom(Window* window) : PostProcessEffect(window),
		   _shader(window, "Resource/Shader/bloom.comp")
	{

	}

	void Bloom::RenderPass(Target2D& t, GL::Texture2D& in, GL::Texture2D& out)
	{
		_shader.Bind();

		u8 mipCount = glm::min<u8>(in.GetMipCount(), BLOOM_MAX_ITERATIONS);
		glm::vec4 settings { BLOOM_THRESHOLD, BLOOM_KNEE, 0, 0 }; // Threshold, Knee, Mode, MIP


		_shader.SetUniform(0, out.Use(0));

		out.CopyFrom(in);

		// Downsample
		for(u8 i = 1; i < mipCount; i++)
		{
			settings.z = i == 1 ? (float) BloomStage::PrefilterDownsample : (float) BloomStage::Downsample;
			settings.w = i - 1.0;

			_shader.SetUniform(1, settings);
			out.Bind(1, GL_WRITE_ONLY, i);

			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			_shader.Dispatch(DIV_CEIL_T(out.GetSize(i), BLOOM_GROUP_SIZE, GL::TextureSize2D));
		}

		// Upsample
		settings.z = (float) BloomStage::Upsample;
		for(u8 i = 1; i < mipCount; i++)
		{
			u8 mip = mipCount - i - 1;

			settings.w = mip + 1.f;

			_shader.SetUniform(1, settings);

			if(mip == 0) in.Bind(0, GL_READ_ONLY, 0);
			else out.Bind(0, GL_READ_ONLY, mip);

			out.Bind(1, GL_WRITE_ONLY, mip);

			glMemoryBarrier(GL_ALL_BARRIER_BITS);
			_shader.Dispatch(DIV_CEIL_T(out.GetSize(mip), BLOOM_GROUP_SIZE, GL::TextureSize2D));
		}
	}
}