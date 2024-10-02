//
// Created by scion on 10/1/2024.
//

#pragma once

#include "DrawCall.h"

namespace gE
{
	inline DrawCall::DrawCall(DrawCallManager& manager) : Managed(&manager, *this)
	{

	}
}
