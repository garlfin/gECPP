//
// Created by scion on 9/5/2024.
//

#pragma once

#include "Texture.h"

namespace GPU
{
	template<Dimension T>
	u8 GetMipCount(const Size<T>& size)
	{
		u32 largest;

		if constexpr(T == Dimension::D1D) largest = size;
		else if constexpr(T == Dimension::D2D) largest = glm::max(size.x, size.y);
		else largest = glm::max(size.x, glm::max(size.y, size.z));

#ifdef GE_COMPILER_GCC
		return 32 - __builtin_clz(largest);
#endif

#ifdef GE_COMPILER_MSVC
		return 32 - __lzcnt(largest);
#endif
	}
}
