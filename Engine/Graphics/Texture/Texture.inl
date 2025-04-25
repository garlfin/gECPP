//
// Created by scion on 9/5/2024.
//

#pragma once

#include "Texture.h"

namespace GPU
{
	template<Dimension DIMENSION>
	u8 GetMipCount(const Size<DIMENSION>& size)
	{
		u32 largest;

		if constexpr(DIMENSION == Dimension::D1D) largest = size;
		else if constexpr(DIMENSION == Dimension::D2D) largest = max(size.x, size.y);
		else largest = max(size.x, max(size.y, size.z));

#ifdef GE_COMPILER_GCC
		return 32 - __builtin_clz(largest);
#endif

#ifdef GE_COMPILER_MSVC
		return 32 - __lzcnt(largest);
#endif
	}

	template <Dimension DIMENSION> requires ((u8) DIMENSION > 1)
	u8 GetMipCountArray(const Size<DIMENSION>& size)
	{
		const Size<(Dimension) ((u8) DIMENSION - 1)> pixelPart = size;
		return GetMipCount<(Dimension) ((u8) DIMENSION - 1)>(pixelPart);
	}
}
