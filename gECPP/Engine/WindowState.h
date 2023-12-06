//
// Created by scion on 9/29/2023.
//

#pragma once

namespace gE
{
	enum class WriteMode : u8
	{
		None,
		Depth,
		Color,
		Both = Depth | Color
	};

	enum class VoxelWriteMode : u8
	{
		None,
		Read,
		Write
	};

	struct RenderFlags
	{
		WriteMode WriteMode : 3;
		VoxelWriteMode VoxelWriteMode : 2;
		bool EnableJitter: 1;
		bool EnableSSEffects: 1;
		bool EnableSpecular : 1;
	};

	namespace State
	{
		static CONSTEXPR_GLOBAL RenderFlags Color { WriteMode::Color, VoxelWriteMode::Read, true, true, true };
		static CONSTEXPR_GLOBAL RenderFlags PreZ { WriteMode::Depth, VoxelWriteMode::None, true, false, false };
		static CONSTEXPR_GLOBAL RenderFlags Shadow { WriteMode::Depth, VoxelWriteMode::None, false, false, false };
		static CONSTEXPR_GLOBAL RenderFlags Cubemap { WriteMode::Both, VoxelWriteMode::None, false, false, false };
		static CONSTEXPR_GLOBAL RenderFlags Voxel { WriteMode::None, VoxelWriteMode::Write, false, false, false };
	}
}