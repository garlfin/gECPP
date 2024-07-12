//
// Created by scion on 9/19/2023.
//

#pragma once

#include "GL/Math.h"
#include "GL/Texture/Texture.h"
#include "GL/Buffer/Buffer.h"
#include "Engine/Renderer/DefaultPipeline.h"
#include "Engine/Component/Camera/Camera.h"

namespace GL
{
	struct SDFScene
	{
		glm::vec3 Center;
		float Scale;
		GL_ALIGN handle Color;
	};
}

namespace gE
{
	class SDFCapture;
}

namespace gE::SDFPipeline
{
	struct TargetSettings
	{

	};

	struct Buffers
	{
	 public:
		explicit Buffers(Window* window);

		ALWAYS_INLINE void UpdateScene(u64 size = sizeof(GL::SDFScene), u64 offset = 0) const
		{
			_voxelBuffer.ReplaceData((u8*) &Scene + offset, size, offset);
		}

		GL::SDFScene Scene;

	 private:
		GL::Buffer<GL::SDFScene> _voxelBuffer;
	};

	CONSTEXPR_GLOBAL GL::ITextureSettings ColorFormat { GL_RGB10_A2, GL::WrapMode::Clamp, GL::FilterMode::Linear, 1 };
	CONSTEXPR_GLOBAL GL::ITextureSettings SDFFormat { GL_RG16F, GL::WrapMode::Clamp, GL::FilterMode::Nearest, 1 };

	class Target3D : public RenderTarget<Camera3D>
	{
	 public:
		explicit Target3D(SDFCapture&, Camera3D&);

		GET(GL::Texture3D&, Color, _color);
		GET(GL::Texture3D&, SDF, _sdf);
		GET(GL::Texture3D&, SDFBack, _sdfBack);

		GET(float, Scale, GetCamera().GetScale());
		GET(SDFCapture&, Owner, (SDFCapture&) RenderTarget<Camera3D>::GetOwner());

		bool Setup(float d, Camera* camera) override;
		void RenderPass(float, Camera*) override;
		void PostProcessPass(float d) override;

	 private:
		GL::Texture3D _color;
		GL::Texture3D _sdf, _sdfBack;
	};
}