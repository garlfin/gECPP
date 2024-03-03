//
// Created by scion on 2/26/2024.
//

#pragma once

#include <GL/Math.h>
#include <gETF/Serializable.h>
#include <gECPP/Prototype.h>
#include <Engine/AssetManager.h>

namespace gETF::UI
{
	enum class SliceMode : u8
	{
		Stretch,
		Continuous
	};

	struct NineSlice : public Serializable<File*>
	{
		glm::vec2 Border;
		SliceMode Mode;
	};

	enum class TransformMode : u8
	{
		Absolute,
		Percent
	};

	struct TransformSettings
	{
		TransformMode PositionX : 1, PositionY : 1;
		TransformMode ScaleX : 1, ScaleY : 1;
	};

	struct Transform2D
	{
		glm::vec2 Position;
		glm::vec2 Scale;

		TransformSettings Mode;
		float Rotation;
	};
}
