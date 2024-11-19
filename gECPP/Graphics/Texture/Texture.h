//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include <Serializable/Asset.h>
#include <Serializable/Macro.h>
#include "TextureSettings.h"

namespace GL
{
	class Texture;
	class Texture2D;
	class Texture3D;
	class TextureCube;
}

namespace GPU
{
	template<Dimension T>
	u8 GetMipCount(const Size<T>& size);

	class Texture : public Serializable<gE::Window*>, public gE::Asset
	{
		SERIALIZABLE_PROTO(TEX, 1, Texture, Serializable);
		API_UNDERLYING();
		DEFAULT_OPERATOR_CM(Texture);

	public:
		ALWAYS_INLINE void Free() override { Data.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return Data.IsFree(); }
		constexpr operator bool() const { return (bool) Format; }

		GLenum Format = GL_NONE;
		WrapMode WrapMode = WrapMode::Repeat;
		FilterMode Filter = FilterMode::Linear;
		u8 MipCount = 1;

		TextureData Data = DEFAULT;

		constexpr ~Texture() override { ASSET_CHECK_FREE(Texture); }
	};

 	class Texture2D : public Texture
	{
 		SERIALIZABLE_PROTO(TEX2, 1, Texture2D, Texture);
 		API_REFLECTABLE(Texture2D, "GPU::Texture2D", API::Texture2D);

 	public:
 		Texture2D(const Texture& super, TextureSize2D size) : Texture(super), Size(size) {};
 		Texture2D(Texture&& super, TextureSize2D size) : Texture(move(super)), Size(size) {};

		TextureSize2D Size;
	};

	class Texture3D : public Texture
	{
		SERIALIZABLE_PROTO(TEX3, 1, Texture3D, Texture);
		API_REFLECTABLE(Texture3D, "GPU::Texture3D", API::Texture3D);

	public:
		Texture3D(const Texture& super, TextureSize3D size) : Texture(super), Size(size) {};
		Texture3D(Texture&& super, TextureSize3D size) : Texture(move(super)), Size(size) {};

		TextureSize3D Size;
	};

	class TextureCube : public Texture
	{
		SERIALIZABLE_PROTO(TEXC, 1, TextureCube, Texture);
		API_REFLECTABLE(TextureCube, "GPU::TextureCube", API::TextureCube);

	public:
		TextureCube(const Texture& super, TextureSize1D size) : Texture(super), Size(size) {};
		TextureCube(Texture&& super, TextureSize1D size) : Texture(move(super)), Size(size) {};

		TextureSize1D Size;
	};
}

#include "Texture.inl"

#include <Graphics/API/GL/Texture/Texture.h>