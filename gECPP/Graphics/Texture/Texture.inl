//
// Created by scion on 9/5/2024.
//

#pragma once

#include "Texture.h"

#define GPU_TEXTURE_DEFINITION(TYPE, DIMENSION) \
	inline TYPE::TYPE(gE::Window* w, const GPU::TextureSettings<DIMENSION>& s, TextureData&& d) : \
		Texture(w, s, std::move(d)), Size(s.Size) \
	{ if(!Settings.MipCount) Settings.MipCount = GetMipCount<DIMENSION>(Size); } \
	inline void TYPE::ISerialize(istream& in, SETTINGS_T s) { Read(in, Size); } \
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

		return 32 - __builtin_clz(largest);
	}

	inline Texture::Texture(gE::Window* window, const ITextureSettings& settings, TextureData&& data) :
		Settings(settings), Data(std::move(data))
	{
	}

	inline void Texture::ISerialize(istream& in, SETTINGS_T s)
	{
		Read(in, Settings);
		Read(in, Data);
	}

	inline void Texture::IDeserialize(ostream& out) const
	{
		Write(out, Settings);
		Write(out, Data);
	}

	GPU_TEXTURE_DEFINITION(Texture2D, Dimension::D2D);
	GPU_TEXTURE_DEFINITION(Texture3D, Dimension::D3D);
	GPU_TEXTURE_DEFINITION(TextureCube, Dimension::D1D);
}
