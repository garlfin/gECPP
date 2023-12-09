//
// Created by scion on 12/4/2023.
//

#include "Tonemap.h"

namespace gE::DefaultPipeline
{
	Tonemap::Tonemap(Window* window) : PostProcessEffect<Target2D>(window),
		_shader(window, "Resource/Shader/tonemap.comp")
	{
	}

	void Tonemap::RenderPass(DefaultPipeline::Target2D& t, GL::Texture2D& in, GL::Texture2D& out)
	{
		_shader.Bind();

		in.Bind(0, GL_READ_ONLY);
		out.Bind(1, GL_WRITE_ONLY);

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		_shader.Dispatch(DIV_CEIL_T(t.GetSize(), TONEMAP_GROUP_SIZE, GL::TextureSize2D));
	}
}