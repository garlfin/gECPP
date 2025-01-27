//
// Created by scion on 12/4/2023.
//

#pragma once

#include <Engine/Component/Camera/PostProcessEffect.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include <Graphics/Shader/Shader.h>

#define TONEMAP_GROUP_SIZE 8

namespace gE::DefaultPipeline
{
	class Tonemap final : public PostProcessEffect<API::Texture2D, PhysicalCameraSettings*>
	{
	 public:
		POSTPROCESS_CONSTRUCTOR(Tonemap, PhysicalCameraSettings*);

		void RenderPass(API::Texture2D& in, API::Texture2D& out) override;
	};
}