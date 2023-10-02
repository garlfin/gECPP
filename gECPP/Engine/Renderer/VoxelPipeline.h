//
// Created by scion on 9/19/2023.
//

#pragma once

#include <GL/Math.h>
#include <GL/Texture/Texture.h>
#include <GL/Buffer/Buffer.h>
#include <Engine/Component/Camera.h>

namespace GL
{
	struct VoxelScene
	{
		glm::vec3 Minimum {};
		float VoxelScale = 0;
		glm::vec3 Maximum {};
		GL_ALIGN u64 Texture = 0;
	};
}

namespace gE
{
	class VoxelCamera;
}

namespace gE::VoxelPipeline
{
	void RenderPass3D(Window*, VoxelCamera*);

	GLOBAL gE::AttachmentSettings Target3D
	{
		{ GL_NONE }, // Depth Format
		{ { GL_RGB16F }, { GL_RGB5_A1 } }, // Attachments: COLOR, DATA (Metallic, Roughness, Specular, Solid)
	};

	struct Buffers
	{
		explicit Buffers(Window* window);

		ALWAYS_INLINE void UpdateScene(u64 size = sizeof(GL::Camera), u64 offset = 0) const
		{
			_voxelBuffer.ReplaceData((u8*) &Scene + offset, size, offset);
		}

		GL::VoxelScene Scene;

	 private:
		GL::Buffer<GL::VoxelScene> _voxelBuffer;
	};
}