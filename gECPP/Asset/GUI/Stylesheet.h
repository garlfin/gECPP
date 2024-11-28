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

	struct TransformModes
	{
		TransformMode PosX : 2;
		TransformMode PosY : 2;
		TransformMode ScaleX : 2;
		TransformMode ScaleY : 2;
	};

	struct Transform
	{
		union
		{
			glm::vec2 Location;
			glm::vec2 Position;
		};

		glm::vec2 Size;

		float Rotation;
		u8 Layer = 0;
		TransformModes TransformMode;
	};

	struct FrameStyle
	{

	};

	struct ButtonStyle
	{

	};

	struct Style
	{
		Transform Transform;
		//gE::SmartPointer<FrameStyle> FrameStyle;
	};


}
