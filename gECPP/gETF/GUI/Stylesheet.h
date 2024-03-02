//
// Created by scion on 2/26/2024.
//

#pragma once

#include "GL/Math.h"
#include "gETF/Serializable.h"

#define STYLE_NAME_MAX_LENGTH 16

namespace gETF::UI
{
	enum class SliceMode : u8
	{
		Stretch,
		Continuous
	};

	enum class TransformMode : u8
	{
		Absolute,
		Percent
	};

	struct NineSlice
	{
		glm::vec2 Border;
		SliceMode Mode;
		gE::Reference<GL::Texture2D> Texture;
	};

	struct TransformSettings
	{
		TransformMode PositionX : 1, PositionY : 1;
		TransformMode ScaleX : 1, ScaleY : 1;
	};

	struct Transform2D : public Serializable<>
	{
		SERIALIZABLE_PROTO;

		glm::vec2 RelativePosition;
		glm::i32vec2 AbsolutePosition;

		glm::vec2 RelativeScale;
		glm::i32vec2 AbsoluteScale;

		TransformSettings Mode;
		float Rotation;
	};
}
