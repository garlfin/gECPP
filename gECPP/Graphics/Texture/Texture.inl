//
// Created by scion on 9/5/2024.
//

#pragma once

#include "Texture.h"

#define GPU_TEXTURE_DEFINITION(TYPE, DIMENSION) \
	inline void TYPE::ISerialize(istream& in, SETTINGS_T s) { Read(in, Size); if(!MipCount) MipCount = GetMipCount<DIMENSION>(Size); } \
	inline void TYPE::IDeserialize(ostream& out) const { Write(out, Size); }

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

	inline void Texture::ISerialize(istream& in, SETTINGS_T s)
	{
		Read(in, Format);
		Read(in, WrapMode);
		Read(in, Filter);
		Read(in, MipCount);
		ReadSerializable(in, Data, nullptr);
	}

	inline void Texture::IDeserialize(ostream& out) const
	{
		Write(out, Format);
		Write(out, WrapMode);
		Write(out, Filter);
		Write(out, MipCount);
		Write(out, Data);
	}

	GPU_TEXTURE_DEFINITION(Texture2D, Dimension::D2D);
	GPU_TEXTURE_DEFINITION(Texture3D, Dimension::D3D);
	GPU_TEXTURE_DEFINITION(TextureCube, Dimension::D1D);
}
