//
// Created by scion on 9/19/2023.
//

#pragma once

#include <GL/Math.h>
#include "GL/Texture/Texture.h"
#include "GL/Buffer/Buffer.h"
#include "Engine/Component/Camera.h"

namespace VoxelDemo
{
	class DemoWindow;
	class VoxelCamera;
}

namespace VoxelDemo::VoxelPipeline
{
	void VoxelRenderPass(DemoWindow*, VoxelCamera*);

	static const gE::AttachmentSettings Target3D
	{
		{ GL_NONE }, // Depth Format
		{ { GL_RGB16F }, { GL_RGB5_A1 } }, // Attachments: COLOR, DATA(Roughness, Metallic, _, SOLID)
	};

	struct VoxelGrid
	{
		glm::vec3 Minimum {};
		float VoxelScale = 0;
		glm::vec3 Maximum {};
		float MipCount = 0;
		u64 Texture = 0;
	};

	struct Buffers
	{
		explicit Buffers(DemoWindow* window);

		VoxelGrid VoxelGrid{};
		GL::Texture3D VoxelTexture;
		GL::Buffer<struct VoxelGrid> GridBuffer;
	};
}