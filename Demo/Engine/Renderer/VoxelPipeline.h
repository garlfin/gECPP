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
	struct VoxelGrid
	{
		glm::vec3 Minimum {};
		float VoxelScale = 0;
		glm::vec3 Maximum {};
		float MipCount = 0;
		u64 Texture = 0;
	};

 	class VoxelCamera : public gE::Camera
	{

	};

	struct VoxelPipeline
	{
		VoxelPipeline();

		VoxelGrid VoxelGrid;
		GL::Texture3D VoxelTexture;
		GL::Buffer<struct VoxelGrid> GridBuffer;
	};
}