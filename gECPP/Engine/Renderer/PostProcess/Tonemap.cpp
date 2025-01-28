//
// Created by scion on 12/4/2023.
//

#include "Tonemap.h"
#include <Engine/Window/Window.h>

namespace gE::PostProcess
{
	bool AutoExposure::RenderPass(GL::Texture2D& in, GL::Texture2D& out)
	{

		return false;
	}

	bool Tonemap::RenderPass(API::Texture2D& in, API::Texture2D& out)
	{
		const API::ComputeShader& shader = GetWindow().GetTonemapShader();

		float exposure = GetSettings().Exposure;
		if(GetSettings().ExposureMode == ExposureMode::Physical)
			exposure = GetSettings().CalculatePhysicalExposure();

		shader.Bind();
		shader.SetUniform(0, exposure);

		in.Bind(0, GL_READ_ONLY);
		out.Bind(1, GL_WRITE_ONLY);

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		GetWindow().GetTonemapShader().Dispatch(DIV_CEIL_T(in.GetSize(), TONEMAP_GROUP_SIZE, TextureSize2D));

		return true;
	}
}