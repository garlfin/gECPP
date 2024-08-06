//
// Created by scion on 2/26/2024.
//

#pragma once

#include "Engine/Math/Math.h"
#include <gETF/Serializable.h>
#include <Engine/AssetManager.h>

namespace gETF::UI
{
	struct Transform
	{
		glm::vec4 Size;
		float Rotation;

		GET_SET_VALUE(float, Left, Size.x);
		GET_SET_VALUE(float, Right, Size.y);
		GET_SET_VALUE(float, Top, Size.z);
		GET_SET_VALUE(float, Bottom, Size.w);
	};

	struct Style
	{
		Transform Transform;
	};

	struct FrameStyle : public Style
	{

	};
}