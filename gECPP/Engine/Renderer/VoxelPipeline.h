//
// Created by scion on 9/19/2023.
//

#pragma once

#include <GL/Math.h>
#include <GL/Texture/Texture.h>
#include <GL/Buffer/Buffer.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include <Engine/Component/Camera/Camera.h>

namespace GL
{
	struct VoxelScene
	{
		glm::vec3 Center;
		float Scale;
		u32 CellCount;
		handle Color;
		GL_ALIGN handle Data;
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

	CONSTEXPR_GLOBAL GL::ITextureSettings ColorFormat { GL_R11F_G11F_B10F, GL::WrapMode::Clamp, GL::FilterMode::Linear, 0 };
	CONSTEXPR_GLOBAL GL::ITextureSettings DataFormat { GL_R8UI, GL::WrapMode::Clamp, GL::FilterMode::Nearest, 0 };

	class Target3D : public RenderTarget<Camera3D>
	{
	 public:
		explicit Target3D(VoxelCapture&, Camera3D&);

		GET(GL::Texture3D&, Color, _color);
		GET(GL::Texture3D&, Data, _data);
		GET(VoxelCapture&, Owner, (VoxelCapture&) RenderTarget<Camera3D>::GetOwner());

		bool Setup(float d, Camera* camera) override;
		void RenderPass(float, Camera*) override;

	 private:
		GL::Texture3D _color;
		GL::Texture3D _data;
	};
}