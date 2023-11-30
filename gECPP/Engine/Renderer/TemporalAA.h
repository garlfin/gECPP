//
// Created by scion on 11/28/2023.
//

#pragma once

#include <Engine/Component/CameraPostProcess.h>
#include <GL/Shader/Shader.h>

#define TAA_GROUP_SIZE 32
#define TAA_VELOCITY(CAM) (*(CAM).GetAttachment<1>())

namespace gE
{
	CONSTEXPR_GLOBAL AttachmentSettings TAARequirements
	{
		GL::SizelessTextureSettings{},
		GL::SizelessTextureSettings{ GL_RG16F, GL::WrapMode::Clamp },
	};

	class TemporalAA : public PostProcessEffect
	{
	 public:
		explicit TemporalAA(Window*);
		void RenderPass(Camera&, GL::Texture&, GL::Texture&) override;

	 private:
		GL::ComputeShader _shader;
	};
}
