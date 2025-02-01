//
// Created by scion on 9/5/2024.
//

#include "Texture.h"
#include "TextureSlotManager.h"

namespace GPU
{
	TextureData::TextureData(GLenum format, GLenum type, CompressionScheme scheme, u8 mip, Array<u8>&& arr) :
		PixelFormat(format), PixelType(type), Scheme(scheme), MipCount(mip), Data(std::move(arr))
	{
	}

	void TextureData::IDeserialize(std::istream& in, SETTINGS_T)
	{
		PixelFormat = Read<GLenum>(in);
		PixelType = Read<GLenum>(in);
		Scheme = Read<CompressionScheme>(in);
		Read<u64, u8>(in, Data);
	}

	void TextureData::ISerialize(std::ostream& out) const
	{
		Write(out, PixelFormat);
		Write(out, PixelType);
		Write(out, Scheme);
		WriteArray<u64>(out, Data);
	}

	u8 TextureSlotManager::Increment(const API::Texture* t)
	{
		// TODO: Consider something O(1)
		// for(u8 i = 0; i < GL_MAX_TEXTURE_SLOT; i++)
		// 	if(_textures[i] == t)
		// 		return i;
		if(!t) return 0;
		t->Use(_index);

		_textures[_index] = t;
		_index++;

		GE_ASSERTM(_index < GE_MAX_TEXTURE_SLOT, "TEXTURE SLOT OVERFLOW!");
		return _index - 1;
	}

	SERIALIZABLE_REFLECTABLE_IMPL(Texture1D, API::Texture1D);
	SERIALIZABLE_REFLECTABLE_IMPL(Texture2D, API::Texture2D);
	SERIALIZABLE_REFLECTABLE_IMPL(Texture3D, API::Texture3D);
	SERIALIZABLE_REFLECTABLE_IMPL(TextureCube, API::TextureCube);
}