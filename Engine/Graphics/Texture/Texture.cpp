//
// Created by scion on 9/5/2024.
//

#include "Texture.h"
#include "TextureSlotManager.h"

#define GPU_TEXTURE_DEFINITION(TYPE, DIMENSION) \
	void TYPE::IDeserialize(istream& in, SETTINGS_T s) { Read(in, Size); if(!MipCount) MipCount = GetMipCount<DIMENSION>(Size); } \
	void TYPE::ISerialize(ostream& out) const { Write(out, Size); }

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
		MipCount = Read<u8>(in);
		Read<u64, std::byte>(in, Data);
	}

	void TextureData::ISerialize(std::ostream& out) const
	{
		Write(out, PixelFormat);
		Write(out, PixelType);
		Write(out, Scheme);
		Write(out, MipCount);
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

	void Texture::IDeserialize(istream& in, SETTINGS_T s)
	{
		Read(in, Format);
		Read(in, WrapMode);
		Read(in, Filter);
		Read(in, MipCount);
		ReadSerializable(in, Data, nullptr);
	}

	void Texture::ISerialize(ostream& out) const
	{
		Write(out, Format);
		Write(out, WrapMode);
		Write(out, Filter);
		Write(out, MipCount);
		Write(out, Data);
	}

	REFLECTABLE_ONGUI_IMPL(Texture,
		/*gE::DrawField<const GLenum>(gE::ScalarField<GLenum>{ "Format" }, Format, depth);
		gE::DrawField<const u8>(gE::ScalarField<u8>{ "Mip Count" }, MipCount, depth);

		gE::DrawField(gE::EnumField{ "Wrap Mode", "", EWrapMode }, WrapMode, depth);
		gE::DrawField(gE::EnumField{ "Filter", "", EFilterMode }, Filter, depth);*/
	);
	REFLECTABLE_FACTORY_NO_IMPL(Texture);

	GPU_TEXTURE_DEFINITION(Texture1D, Dimension::D1D);
	REFLECTABLE_ONGUI_IMPL(Texture1D,
		//gE::DrawField(gE::ScalarField<u32>{ "Size" }, (const Size1D&) Size, depth);
	);
	API_REFLECTABLE_FACTORY_IMPL(Texture1D, API::Texture1D);

	GPU_TEXTURE_DEFINITION(Texture2D, Dimension::D2D);
	REFLECTABLE_ONGUI_IMPL(Texture2D,
		//gE::DrawField(gE::ScalarField<u32>{ "Size" }, (const Size2D&) Size, depth);
	)
	API_REFLECTABLE_FACTORY_IMPL(Texture2D, API::Texture2D);

	GPU_TEXTURE_DEFINITION(Texture2DArray, Dimension::D3D);
	REFLECTABLE_ONGUI_IMPL(Texture2DArray,
		//gE::DrawField(gE::ScalarField<u32>{ "Size" }, (const Size3D&) Size, depth);
	);
	API_REFLECTABLE_FACTORY_IMPL(Texture2DArray, API::Texture2DArray);

	GPU_TEXTURE_DEFINITION(Texture3D, Dimension::D3D);
	REFLECTABLE_ONGUI_IMPL(Texture3D,
		//gE::DrawField(gE::ScalarField<u32>{ "Size" }, (const Size3D&) Size, depth);
	);
	API_REFLECTABLE_FACTORY_IMPL(Texture3D, API::Texture3D);

	GPU_TEXTURE_DEFINITION(TextureCube, Dimension::D1D);
	REFLECTABLE_ONGUI_IMPL(TextureCube,
		//gE::DrawField(gE::ScalarField<u32>{ "Size" }, (const Size1D&) Size, depth);
	);
	API_REFLECTABLE_FACTORY_IMPL(TextureCube, API::TextureCube);

	REFLECTABLE_ONGUI_IMPL(TextureData, );
	REFLECTABLE_FACTORY_IMPL(TextureData);
}
