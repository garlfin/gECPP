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
		Both = Depth | Color,
	};

	inline WriteMode operator&(WriteMode a, WriteMode b) { return WriteMode(u8(a) & u8(b)); }

	enum class VoxelWriteMode : u8
	{
		Read,
		Write
	};

	struct RenderFlags
	{
		WriteMode WriteMode : 2;
		VoxelWriteMode VoxelWriteMode : 1;
		bool EnableJitter : 1;
		bool EnableSSEffects : 1;
		bool EnableSpecular : 1;
		bool EnableFaceCull : 1;
		u8 InstanceMultiplier : 3;
	};

	namespace State
	{
		static CONSTEXPR_GLOBAL RenderFlags Color { WriteMode::Color, VoxelWriteMode::Read, true, true, true, true, 1 };
		static CONSTEXPR_GLOBAL RenderFlags PreZ { WriteMode::Depth, VoxelWriteMode::Read, true, false, false, true, 1 };
		static CONSTEXPR_GLOBAL RenderFlags Shadow { WriteMode::Depth, VoxelWriteMode::Read, false, false, false, false, 1 };
		static CONSTEXPR_GLOBAL RenderFlags ShadowCube { WriteMode::Depth, VoxelWriteMode::Read, false, false, false, false, 6 };
		static CONSTEXPR_GLOBAL RenderFlags Cubemap { WriteMode::Both, VoxelWriteMode::Read, false, false, false, true, 6 };
		static CONSTEXPR_GLOBAL RenderFlags Voxel { WriteMode::Color, VoxelWriteMode::Write, false, false, false, false, 3 };
	}
}