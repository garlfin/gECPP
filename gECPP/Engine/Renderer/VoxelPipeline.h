//
// Created by scion on 9/19/2023.
//

#pragma once

#include <Engine/Component/Camera/Camera.h>
#include <Engine/Math/Math.h>
#include <Engine/Renderer/DefaultPipeline.h>
#include <Graphics/Buffer/Buffer.h>
#include <Graphics/Shader/Shader.h>
#include <Graphics/Texture/Texture.h>

namespace GL
{
	struct VoxelScene
	{
		glm::vec3 Center;
		float Scale;
		API_ALIGN handle Color;
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

		ALWAYS_INLINE void UpdateScene(u64 size = sizeof(API::VoxelScene), u64 offset = 0) const
		{
			_voxelBuffer.ReplaceData((u8*) &Scene + offset, size, offset);
		}

		API::VoxelScene Scene;

	private:
		API::Buffer<API::VoxelScene> _voxelBuffer;
	};

	GLOBAL GPU::Texture ColorFormat = []
	{
		GPU::Texture tex;
		tex.Format = GL_RGB10_A2;
		tex.WrapMode = GPU::WrapMode::Clamp;
		tex.MipCount = 0;
		return tex;
	}();

	GLOBAL GPU::Texture ColorBackFormat = []
	{
		GPU::Texture tex;
		tex.Format = GL_RGB10_A2;
		tex.WrapMode = GPU::WrapMode::Clamp;
		tex.Filter = GPU::FilterMode::Nearest, 1;
		return tex;
	}();

	class Target3D : public RenderTarget<Camera3D>
	{
	 public:
		explicit Target3D(VoxelCapture&, Camera3D&);

		GET(API::Texture3D&, Color, _color);
		GET(float, Scale, GetCamera().GetScale());
		GET(VoxelCapture&, Owner, (VoxelCapture&) RenderTarget<Camera3D>::GetOwner());

		bool Setup(float d, Camera* camera) override;
		void RenderPass(float, Camera*) override;
		void PostProcessPass(float d) override;

	 private:
		glm::ivec3 _velocity = DEFAULT;
		API::Texture3D _color, _colorBack;
	};
}