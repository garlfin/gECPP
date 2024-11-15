//
// Created by scion on 2/26/2024.
//

#pragma once

#include "Engine/Math/Math.h"

namespace gETF::UI
{
	enum class TransformMode : u8
	{
		Relative,
		Absolute
	};

	struct Transform
	{
		glm::vec2 Size;
		float Rotation;
		i16 Layer = INT16_MIN;

		TransformMode ModeX;
		TransformMode ModeY;
	};

	struct FrameStyle
	{
		Transform Transform;
	};

	struct Style
	{
		Transform Transform;
		gE::SmartPointer<FrameStyle> FrameStyle;
	};


}
