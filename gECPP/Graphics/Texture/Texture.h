//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include "TextureSettings.h"

namespace GPU
{
	template<Dimension T>
	u8 GetMipCount(const Size<T>& size);

	class Texture : public Serializable<gE::Window*>, public Asset
	{
		SERIALIZABLE_PROTO(TEX, 1, Texture, Serializable);

	public:
		Texture(gE::Window*, const ITextureSettings&, TextureData&&);

		GET_CONST(const ITextureSettings&, Settings, Settings);
		GET_CONST(const TextureData&, Data, Data);

		ALWAYS_INLINE void Free() override { Data.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return Data.IsFree(); }

		ITextureSettings Settings;
		TextureData Data;

		~Texture() override { ASSET_CHECK_FREE(Texture); }
	};

 	class Texture2D : public Texture
	{
 		SERIALIZABLE_PROTO(TEX2, 1, Texture2D, Texture);

 	public:
 		Texture2D(gE::Window*, const TextureSettings2D&, TextureData&&);

 		GET_CONST(TextureSize2D, Size, Size);

		TextureSize2D Size;
	};

	class Texture3D : public Texture
	{
		SERIALIZABLE_PROTO(TEX3, 1, Texture3D, Texture);

	public:
		Texture3D(gE::Window*, const TextureSettings3D&, TextureData&&);

		GET_CONST(TextureSize3D, Size, Size);

		TextureSize3D Size;
	};

	class TextureCube : public Texture
	{
		SERIALIZABLE_PROTO(TEXC, 1, TextureCube, Texture);

	public:
		TextureCube(gE::Window*, const TextureSettings1D&, TextureData&&);

		GET_CONST(TextureSize1D, Size, Size);

		TextureSize1D Size;
	};
}

#include "Texture.inl"

#include <Graphics/API/GL/Texture/Texture.h>
