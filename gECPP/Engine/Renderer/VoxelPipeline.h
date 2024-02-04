//
// Created by scion on 9/19/2023.
//

#pragma once

#include <GL/Math.h>
#include <GL/Texture/Texture.h>
#include <GL/Buffer/Buffer.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include <Engine/Component/Camera/Camera.h>

#define VOXEL_MAX_MIPS 4

namespace GL
{
	struct VoxelScene
	{
		glm::vec3 Center;
		float Scale;
		handle Color;
	};
}

namespace gE
{
	class VoxelCapture;
}

namespace gE::VoxelPipeline
{
	struct Buffers
	{
	 public:
		explicit Buffers(Window* window);

		ALWAYS_INLINE void UpdateScene(u64 size = sizeof(GL::VoxelScene), u64 offset = 0) const
		{
			_voxelBuffer.ReplaceData((u8*) &Scene + offset, size, offset);
		}

		GL::VoxelScene Scene;

	private:
		GL::Buffer<GL::VoxelScene> _voxelBuffer;
	};

	CONSTEXPR_GLOBAL GL::ITextureSettings ColorFormat { GL_RGB10_A2, GL::WrapMode::Clamp, GL::FilterMode::Linear, VOXEL_MAX_MIPS };
	CONSTEXPR_GLOBAL GL::ITextureSettings ColorBackFormat { GL_RGB10_A2, GL::WrapMode::Clamp, GL::FilterMode::Nearest, 1 };

	class Target3D : public RenderTarget<Camera3D>
	{
	 public:
		explicit Target3D(VoxelCapture&, Camera3D&);

		GET(GL::Texture3D&, Color, _color);
		GET(float, Scale, GetCamera().GetScale());
		GET(VoxelCapture&, Owner, (VoxelCapture&) RenderTarget<Camera3D>::GetOwner());

		bool Setup(float d, Camera* camera) override;
		void RenderPass(float, Camera*) override;
		void PostProcessPass(float d) override;

	 private:
		glm::ivec3 _velocity;
		GL::Texture3D _colorBack;
		GL::Texture3D _color;
	};
}