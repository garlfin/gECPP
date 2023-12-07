//
// Created by scion on 12/4/2023.
//

#pragma once

#include <Engine/Renderer/DefaultPipeline.h>
#include <Engine/Component/Camera/PostProcessEffect.h>
#include <GL/Shader/Shader.h>

namespace gE::DefaultPipeline
{
	class Tonemap : public PostProcessEffect<Target2D>
	{
	 public:
		explicit Tonemap(Window*);

		void RenderPass(DefaultPipeline::Target2D& t, GL::Texture& texture, GL::Texture& texture1) override;

	 private:
		GL::ComputeShader _shader;
	};
}