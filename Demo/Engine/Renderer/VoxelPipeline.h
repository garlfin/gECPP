//
// Created by scion on 9/19/2023.
//

#pragma once

#include <GL/Math.h>

namespace VoxelDemo
{
	struct VoxelGrid
	{
		glm::vec3 Minimum;
		float VoxelScale;
		glm::vec3 Maximum;
		float MipCount;
		u64 Texture;
	};

	struct VoxelPipeline
	{
		VoxelGrid VoxelGrid;
	};
}