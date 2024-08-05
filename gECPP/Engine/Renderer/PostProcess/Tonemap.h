//
// Created by scion on 12/4/2023.
//

#pragma once

#include <Engine/Renderer/DefaultPipeline.h>
#include <Engine/Component/Camera/PostProcessEffect.h>
#include <Graphics/Shader/Shader.h>

#define TONEMAP_GROUP_SIZE 8

namespace gE::DefaultPipeline
{
	class Tonemap : public PostProcessEffect<Target2D>
	{
	 public:
		explicit Tonemap(Target2D&);
		void RenderPass(GL::Texture2D& in, GL::Texture2D& out) override;
	};
}