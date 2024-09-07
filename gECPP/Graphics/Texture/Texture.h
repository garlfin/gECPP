//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include "TextureSettings.h"
#include "Graphics/Macro.h"

namespace GPU
{
	template<Dimension T>
	u8 GetMipCount(const Size<T>& size);

	class Texture : public Serializable<gE::Window*>
	{
	public:
		Texture(gE::Window*, const ITextureSettings&, TextureData&&);
		API_SERIALIZABLE(Texture, Serializable);

		GET_CONST(gE::Window*, Window, _window);
		GET_CONST(const ITextureSettings&, Settings, Settings);
		GET_CONST(const TextureData&, Data, Data);

	protected:
		ITextureSettings Settings;
		TextureData Data;

	private:
		gE::Window* _window;
	};

 	class Texture2D : public Texture
	{
 	public:
 		Texture2D(gE::Window*, const TextureSettings2D&, TextureData&&);
 		API_SERIALIZABLE(Texture2D, Texture);

 		GET_CONST(TextureSize2D, Size, _size);

	private:
		TextureSize2D _size;
	};

	class Texture3D : public Texture
	{
	public:
		Texture3D(gE::Window*, const TextureSettings3D&, TextureData&&);
		API_SERIALIZABLE(Texture3D, Texture);

		GET_CONST(TextureSize3D, Size, _size);

	private:
		TextureSize3D _size;
	};

	class TextureCube : public Texture
	{
	public:
		TextureCube(gE::Window*, const TextureSettings1D&, TextureData&&);
		API_SERIALIZABLE(TextureCube, Texture);

		GET_CONST(TextureSize1D, Size, _size);

	private:
		TextureSize1D _size;
	};
}

#include "Texture.inl"

#include <Graphics/API/GL/Texture/Texture.h>
