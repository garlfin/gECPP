//
// Created by scion on 12/4/2023.
//

#include "Tonemap.h"

#define EXPOSURE_GROUP_SIZE 32

namespace gE::DefaultPipeline
{
	Tonemap::Tonemap(Window* window) : PostProcessEffect<Target2D>(window),
										_shader(window, "Resource/Shader/tonemap.comp")
	{
	}

	void Tonemap::RenderPass(DefaultPipeline::Target2D& t, GL::Texture& in, GL::Texture& out)
	{
		_shader.Bind();

		in.Bind(0, GL_READ_ONLY);
		out.Bind(1, GL_WRITE_ONLY);

		_shader.Dispatch(DIV_CEIL_T(t.GetSize(), EXPOSURE_GROUP_SIZE, GL::TextureSize2D));
	}
}