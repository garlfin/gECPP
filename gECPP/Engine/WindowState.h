//
// Created by scion on 9/29/2023.
//

#pragma once

namespace gE
{
	enum class RenderStage
	{
		Cubemap,
		Voxel,
		PreZ,
		Shadow = PreZ,
		Color,
		Transparent
	};
}