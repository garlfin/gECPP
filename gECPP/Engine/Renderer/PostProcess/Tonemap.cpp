//
// Created by scion on 12/4/2023.
//

#include "Tonemap.h"
#include <Engine/Window/Window.h>

namespace gE::DefaultPipeline
{
	Tonemap::Tonemap(Target2D& target) : PostProcessEffect(target)
	{
	}

	void Tonemap::RenderPass(API::Texture2D& in, API::Texture2D& out)
	{
		API::ComputeShader& shader = GetTarget().GetWindow().GetTonemapShader();
		shader.Bind();

		in.Bind(0, GL_READ_ONLY);
		out.Bind(1, GL_WRITE_ONLY);

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		shader.Dispatch(DIV_CEIL_T(GetTarget().GetSize(), TONEMAP_GROUP_SIZE, TextureSize2D));
	}
}