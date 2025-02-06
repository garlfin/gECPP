//
// Created by scion on 9/29/2023.
//

#pragma once

#include <Prototype.h>
#include <Utility/Macro.h>
#include <Math/Math.h>

namespace gE
{
	enum class WriteMode : u8
	{
		None,
		Depth,
		Color,
		Both = Depth | Color,
	};

	ENUM_OPERATOR(WriteMode, &);
	ENUM_OPERATOR(WriteMode, |);

	enum class RenderMode : u8
	{
		Geometry = 1,
		Fragment = 1 << 1,
		Both = Geometry | Fragment
	};

	ENUM_OPERATOR(RenderMode, &);
	ENUM_OPERATOR(RenderMode, |);

	enum class VoxelWriteMode : u8
	{
		Read,
		Write
	};

	enum class RasterMode : u8
	{
		Normal,
		Conservative
	};

	enum class DepthMode : u8
	{
		Normal,
		Radial
	};

	struct RenderFlags
	{
		RenderMode RenderMode : 2 = RenderMode::Both; // 0
		WriteMode WriteMode : 2 = WriteMode::Both; // 2
		DepthMode DepthMode : 1 = DepthMode::Normal; // 4
		VoxelWriteMode VoxelWriteMode : 1 = VoxelWriteMode::Read; // 5
		RasterMode RasterMode : 1 = RasterMode::Normal; // 6

		BIT_FIELD_ALIGN;
		u8 InstanceMultiplier : 4 = 1; // 8
		bool EnableJitter : 1 = true; // 12
		bool EnableFaceCull : 1 = true; // 13
		bool EnableDepthTest : 1 = true; // 14
		bool EnableSpecular : 1 = true; // 15
	};

	enum class CloseFlags : u8
	{
		Close,
		Restart,
	};

	namespace RenderState
	{
		CONSTEXPR_GLOBAL RenderFlags Forward     = DEFAULT;
		CONSTEXPR_GLOBAL RenderFlags PreZ        = { RenderMode::Geometry, WriteMode::Depth };
		CONSTEXPR_GLOBAL RenderFlags PreZForward = { RenderMode::Fragment, WriteMode::Color };
		CONSTEXPR_GLOBAL RenderFlags GBuffer     = { RenderMode::Geometry };
		CONSTEXPR_GLOBAL RenderFlags Deferred    = { RenderMode::Fragment, WriteMode::Color, DepthMode::Normal, VoxelWriteMode::Read, RasterMode::Normal, 1, false, false, false, true };
		CONSTEXPR_GLOBAL RenderFlags Shadow      = { RenderMode::Geometry, WriteMode::Depth, DepthMode::Normal, VoxelWriteMode::Read, RasterMode::Normal, 1, false, false };
		CONSTEXPR_GLOBAL RenderFlags ShadowCube  = { RenderMode::Geometry, WriteMode::Depth, DepthMode::Radial, VoxelWriteMode::Read, RasterMode::Normal, 6, false, false };
		CONSTEXPR_GLOBAL RenderFlags Cubemap     = { RenderMode::Both, WriteMode::Both, DepthMode::Normal, VoxelWriteMode::Read, RasterMode::Normal, 6, false, false, true, false };
		CONSTEXPR_GLOBAL RenderFlags Voxel       = { RenderMode::Both, WriteMode::Color, DepthMode::Normal, VoxelWriteMode::Write, RasterMode::Normal, 6, false, false, false, false };
	}
}