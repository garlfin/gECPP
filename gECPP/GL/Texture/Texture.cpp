#include "Texture.h"
#include <iostream>
#include <GLAD/glad.h>
#include "Engine/Binary/Binary.h"
#include "TextureSlotManager.h"

CONSTEXPR_GLOBAL float WhiteBorderColor[4] { 1.f, 1.f, 1.f, 0.f };

namespace GL
{
	Texture::Texture(gE::Window* window, GLenum target, const ITextureSettings& settings) :
		Asset(window), Settings(settings), Target(target)
	{
		glCreateTextures(target, 1, &ID);
		glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, (GLint) settings.Filter + (Settings.MipCount == 1 ? 0 : 0x102));
		glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, (GLint) settings.Filter);
		glTextureParameteri(ID, GL_TEXTURE_WRAP_S, (GLint) settings.WrapMode);
		glTextureParameteri(ID, GL_TEXTURE_WRAP_T, (GLint) settings.WrapMode);

		// if(settings.WrapMode == WrapMode::Border)
		//	glTextureParameterfv(ID, GL_TEXTURE_BORDER_COLOR, WhiteBorderColor);

		if(target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_3D)
			glTextureParameteri(ID, GL_TEXTURE_WRAP_R, (GLint) settings.WrapMode);
		else if (target == GL_TEXTURE_2D)
			glTextureParameteri(ID, GL_TEXTURE_MAX_ANISOTROPY, GE_ANISOTROPY_COUNT);
	}


	Texture2D::Texture2D(gE::Window* window, const TextureSettings<TextureDimension::D2D>& settings, TextureData&& data)
		: Texture(window, GL_TEXTURE_2D, settings), _size(settings.Size)
	{
		if(!Settings.MipCount) Settings.MipCount = ::GL::GetMipCount<TextureDimension::D2D>(_size);
		glTextureStorage2D(ID, Settings.MipCount, Settings.Format, _size.x, _size.y);

		if(!data.Data) return;

		glm::u32vec2 size = _size;
		u8* dataPtr = data.Data.Data();

		for(u8 i = 0; i < data.MipCount; i++, size >>= decltype(size)(1)) // lazy guy
		{
			size = glm::max(size, decltype(size)(1)); // double lazy guy
			u64 dataSize = data.Scheme.Size<TextureDimension::D2D>(size);

			if(data.Scheme.IsCompressed())
				glCompressedTextureSubImage2D(ID, i, 0, 0, size.x, size.y, Settings.Format, dataSize, dataPtr);
			else
				glTextureSubImage2D(ID, i, 0, 0, size.x, size.y, data.PixelFormat, data.PixelType, dataPtr);

			dataPtr += dataSize;
		}

		if(Settings.MipCount > 1 && !data.MipCount) glGenerateTextureMipmap(ID);
	}

	void Texture2D::CopyFrom(const Texture& o)
	{
		glCopyImageSubData(o.Get(), GL_TEXTURE_2D, 0, 0, 0, 0, ID, GL_TEXTURE_2D, 0, 0, 0, 0, GetSize().x, GetSize().y, 1);
	}

	Texture3D::Texture3D(gE::Window* window, const TextureSettings<TextureDimension::D3D>& settings, TextureData&& data)
		: Texture(window, GL_TEXTURE_3D, settings), _size(settings.Size)
	{
		if(!Settings.MipCount) Settings.MipCount = ::GL::GetMipCount<TextureDimension::D3D>(_size);
		glTextureStorage3D(ID, Settings.MipCount, Settings.Format, _size.x, _size.y, _size.z);

		if(!data.Data) return;

		glm::u32vec3 size = _size;
		u8* dataPtr = data.Data.Data();

		for(u8 i = 0; i < data.MipCount; i++, size >>= decltype(size)(1)) // lazy guy
		{
			size = glm::max(size, decltype(size)(1)); // double lazy guy
			u64 dataSize = data.Scheme.Size<TextureDimension::D3D>(size);

			if(data.Scheme.IsCompressed())
				glCompressedTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, Settings.Format, dataSize, dataPtr);
			else
				glTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, data.PixelFormat, data.PixelType, dataPtr);

			dataPtr += dataSize;
		}

		if(Settings.MipCount > 1 && !data.MipCount) glGenerateTextureMipmap(ID);
	}

	void Texture3D::CopyFrom(const Texture& o)
	{
		glCopyImageSubData(o.Get(), GL_TEXTURE_3D, 0, 0, 0, 0, ID, GL_TEXTURE_3D, 0, 0, 0, 0, GetSize().x, GetSize().y, GetSize().z);
	}

	Texture::~Texture()
	{
		if(_handle) glMakeTextureHandleNonResidentARB(_handle);
		glDeleteTextures(1, &ID);
	}

	handle Texture::GetHandle()
	{
		if(_handle) return _handle;
		if(!GLAD_GL_ARB_bindless_texture) return _handle = 1;

		_handle = glGetTextureHandleARB(ID);
		glMakeTextureHandleResidentARB(_handle);

		return _handle;
	}

	TextureCube::TextureCube(gE::Window* window, const TextureSettings<TextureDimension::D1D>& settings, TextureData&& data)
		: Texture(window, GL_TEXTURE_CUBE_MAP, settings), _size(settings.Size)
	{
		if(!Settings.MipCount) Settings.MipCount = ::GL::GetMipCount<TextureDimension::D1D>(_size);
		glTextureStorage2D(ID, Settings.MipCount, settings.Format, _size, _size);

		if(!data.Data) return;

		u32 size = _size;
		u8* dataPtr = (u8*) data.Data.Data(); // sobbing rn

		for(u8 i = 0; i < data.MipCount; i++, size >>= 1)
		{
			size = glm::max(size, 1u);
			u64 dataSize = data.Scheme.Size<TextureDimension::D3D>(TextureSize3D(size, size, 6));

			if(data.Scheme.IsCompressed())
				glCompressedTextureSubImage3D(ID, i, 0, 0, 0, size, size, 6, settings.Format, dataSize, dataPtr);
			else
				glTextureSubImage3D(ID, i, 0, 0, 0, size, size, 6, data.PixelFormat, data.PixelType, dataPtr);

			dataPtr += dataSize;
		}

		if(Settings.MipCount > 1 && !data.MipCount) glGenerateTextureMipmap(ID);
	}

	u8 TextureSlotManager::Increment(const GL::Texture* t)
	{
		// TODO: Consider something O(1)
		// for(u8 i = 0; i < GL_MAX_TEXTURE_SLOT; i++)
		// 	if(_textures[i] == t)
		// 		return i;
		if(!t) return 0;
		t->Use(_index);

		_textures[_index] = t;
		_index++;

		GE_ASSERT(_index < GL_MAX_TEXTURE_SLOT, "TEXTURE SLOT OVERFLOW!");
		return _index - 1;
	}

	void Texture::ISerialize(std::istream& in, gE::Window* s)
	{
		Settings = Read<ITextureSettings>(in);
	}

	void Texture::IDeserialize(std::ostream& out) const
	{
		Write(out, Settings);
	}

	void TextureData::ISerialize(std::istream& in)
	{
		PixelFormat = Read<GLenum>(in);
		PixelType = Read<GLenum>(in);
		Scheme = Read<CompressionScheme>(in);
		Read<u64, u8>(in, Data);
	}

	void TextureData::IDeserialize(std::ostream& out) const
	{
		Write(out, PixelFormat);
		Write(out, PixelType);
		Write(out, Scheme);
		WriteArray<u64>(out, Data);
	}

	TextureData::TextureData(GLenum format, GLenum type, CompressionScheme scheme, u8 mip, Array<u8>&& arr) :
		PixelFormat(format), PixelType(type), Scheme(scheme), MipCount(mip), Data(arr)
	{
	}
}