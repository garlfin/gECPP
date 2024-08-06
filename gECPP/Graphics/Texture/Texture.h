//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Graphics/Graphics.h>
#include "TextureSettings.h"

namespace gE::Graphics
{
	class Texture : public Serializable<gE::Window*>
	{
		SERIALIZABLE_PROTO_T(Texture, Serializable<gE::Window*>);

	 public:
		Texture(gE::Window*, ITextureSettings, TextureData);

	 private:
		ITextureSettings _settings;
		TextureData _data;
	};

 	class Texture2D : public Texture
	{
		SERIALIZABLE_PROTO_T(Texture2D, Texture);
	 public:
	 private:
		TextureSize2D _size;
	};
}

#include <Graphics/API/GL/Texture/Texture.h>
