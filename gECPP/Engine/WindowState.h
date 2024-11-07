//
// Created by scion on 9/29/2023.
//

#pragma once

#include <gECPP/Prototype.h>
#include <Engine/Utility/Macro.h>
#include <Engine/Math/Math.h>

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

	enum class UpdateType : u8
	{
		FixedUpdate,
		Update,
		LateUpdate,
		EarlyFixedUpdate
	};

	enum class RenderType : u8
	{
		PreRender,
		Render,
		PostRender
	};

	struct RenderFlags
	{
		WriteMode WriteMode : 2;
		VoxelWriteMode VoxelWriteMode : 1;
		bool EnableJitter : 1;
		bool EnableSSEffects : 1;
		bool EnableSpecular : 1;
		bool EnableFaceCull : 1;
		bool EnableDepthTest : 1;
		RasterMode RasterMode : 1;
		u8 InstanceMultiplier : 3;
	};

	namespace RenderState
	{
		CONSTEXPR_GLOBAL RenderFlags Color { WriteMode::Color, VoxelWriteMode::Read, true, true, true, true, true, RasterMode::Normal, 1 };
		CONSTEXPR_GLOBAL RenderFlags PreZ { WriteMode::Depth, VoxelWriteMode::Read, true, false, false, true, true, RasterMode::Normal, 1 };
		CONSTEXPR_GLOBAL RenderFlags Shadow { WriteMode::Depth, VoxelWriteMode::Read, false, false, false, false, true, RasterMode::Normal, 1 };
		CONSTEXPR_GLOBAL RenderFlags ShadowCube { WriteMode::Depth, VoxelWriteMode::Read, false, false, false, false, true, RasterMode::Normal, 6 };
		CONSTEXPR_GLOBAL RenderFlags Cubemap { WriteMode::Both, VoxelWriteMode::Read, false, false, false, true, true, RasterMode::Normal, 6 };
		CONSTEXPR_GLOBAL RenderFlags Voxel { WriteMode::Color, VoxelWriteMode::Write, false, false, false, false, false, RasterMode::Normal, 3 };
	}
}