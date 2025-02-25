//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Core/Serializable/Asset.h>
#include <Graphics/Graphics.h>

#include "TextureSettings.h"

namespace GL
{
	class Texture;
	class Texture1D;
	class Texture2D;
	class Texture3D;
	class TextureCube;
}

#if defined(GE_ENABLE_EDITOR) && API_ID == API_GL
	#define TEXTURE_ONGUI_IMPL(API_T) \
		protected: \
			const API::Texture* GetGUITexture() const override { if constexpr(API_ID == API_T) return this; else return nullptr; }
#else
	#define TEXTURE_ONGUI_IMPL(API_T, TYPE)
#endif

namespace GPU
{
	template<Dimension T>
	u8 GetMipCount(const Size<T>& size);

	class Texture : public gE::Asset
	{
		SERIALIZABLE_PROTO("TEX", 1, Texture, Asset);
		REFLECTABLE_ONGUI_PROTO(Asset);

	public:
		ALWAYS_INLINE void Free() override { Data.Free(); }
		NODISCARD ALWAYS_INLINE bool IsFree() const override { return Data.IsFree(); }
		operator bool() const { return (bool) Format; }

		GLenum Format = GL_NONE;
		WrapMode WrapMode = WrapMode::Repeat;
		FilterMode Filter = FilterMode::Linear;
		u8 MipCount = 1;

		TextureData Data = DEFAULT;

		~Texture() override { ASSET_CHECK_FREE(Texture); }

#ifdef GE_ENABLE_EDITOR
	protected:
		virtual const API::Texture* GetGUITexture() const { return nullptr; }
#endif
	};

	class Texture1D : public Texture
	{
		SERIALIZABLE_PROTO("TEX1", 1, Texture1D, Texture);
		REFLECTABLE_PROTO(Texture1D, Texture, "API::Texture1D");

	public:
		Texture1D(const Texture& super, Size1D size) : Texture(super), Size(size) {};
		Texture1D(Texture&& super, Size1D size) : Texture(move(super)), Size(size) {};

		Size1D Size = DEFAULT;
	};

 	class Texture2D : public Texture
	{
 		SERIALIZABLE_PROTO("TEX2", 1, Texture2D, Texture);
 		REFLECTABLE_PROTO(Texture2D, Texture, "GPU::Texture2D");

 	public:
 		Texture2D(const Texture& super, Size2D size) : Texture(super), Size(size) {};
 		Texture2D(Texture&& super, Size2D size) : Texture(move(super)), Size(size) {};

		Size2D Size = DEFAULT;
	};

	class Texture3D : public Texture
	{
		SERIALIZABLE_PROTO("TEX3", 1, Texture3D, Texture);
		REFLECTABLE_PROTO(Texture3D, Texture, "GPU::Texture3D");

	public:
		Texture3D(const Texture& super, Size3D size) : Texture(super), Size(size) {};
		Texture3D(Texture&& super, Size3D size) : Texture(move(super)), Size(size) {};

		Size3D Size = DEFAULT;
	};

	class TextureCube : public Texture
	{
		SERIALIZABLE_PROTO("TEXC", 1, TextureCube, Texture);
		REFLECTABLE_PROTO(TextureCube, Texture, "GPU::TextureCube");

	public:
		TextureCube(const Texture& super, Size1D size) : Texture(super), Size(size) {};
		TextureCube(Texture&& super, Size1D size) : Texture(move(super)), Size(size) {};

		Size1D Size = DEFAULT;
	};
}

#include "Texture.inl"

#if API == GL
	#include <Graphics/API/GL/Texture/Texture.h>
#endif