#include "Texture.h"
#include <GLAD/glad.h>
#include <Engine/Binary/Binary.h>

#include "TextureSlotManager.h"

namespace GL
{
	Texture::Texture(gE::Window* window, GLenum target, const GPU::ITextureSettings& settings) :
		API::APIObject(window), _settings(settings), _target(target)
	{
		glCreateTextures(target, 1, &ID);
		glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, (GLint) settings.Filter + (_settings.MipCount == 1 ? 0 : 0x102));
		glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, (GLint) settings.Filter);
		glTextureParameteri(ID, GL_TEXTURE_WRAP_S, (GLint) settings.WrapMode);
		glTextureParameteri(ID, GL_TEXTURE_WRAP_T, (GLint) settings.WrapMode);

		if(target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_3D)
			glTextureParameteri(ID, GL_TEXTURE_WRAP_R, (GLint) settings.WrapMode);
		else if (target == GL_TEXTURE_2D)
			glTextureParameteri(ID, GL_TEXTURE_MAX_ANISOTROPY, GE_ANISOTROPY_COUNT);
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

	void Texture2D::Construct()
	{
		const GPU::TextureData& data = GetData();
		const u8* imageData = data.Data.Data();
		TextureSize2D size = GetSize();

		glTextureStorage2D(ID, Settings.MipCount, Settings.Format, size.x, size.y);

		if(!imageData) return;
		for(u8 i = 0; i < data.MipCount; i++, size >>= TextureSize2D(1))
		{
			size = max(size, TextureSize2D(1));
			u64 dataSize = data.Scheme.Size<Dimension::D2D>(size);

			if(data.Scheme.IsCompressed())
				glCompressedTextureSubImage2D(ID, i, 0, 0, size.x, size.y, Settings.Format, dataSize, imageData);
			else
				glTextureSubImage2D(ID, i, 0, 0, size.x, size.y, data.PixelFormat, data.PixelType, imageData);

			imageData += dataSize;
		}

		if(Settings.MipCount != 1 && data.MipCount == 1) glGenerateTextureMipmap(ID);

		Free();
	}

	void Texture2D::CopyFrom(const GL::Texture& o)
	{
		glCopyImageSubData(o.Get(), GL_TEXTURE_2D, 0, 0, 0, 0, ID, GL_TEXTURE_2D, 0, 0, 0, 0, GetSize().x, GetSize().y, 1);
	}

	void Texture3D::Construct()
	{
		const GPU::TextureData& data = GetData();
		const u8* imageData = data.Data.Data();
		TextureSize3D size = GetSize();

		glTextureStorage3D(ID, Settings.MipCount, Settings.Format, size.x, size.y, size.z);

		if(!imageData) return;
		for(u8 i = 0; i < data.MipCount; i++, size >>= TextureSize3D(1))
		{
			size = max(size, TextureSize3D(1));
			u64 dataSize = data.Scheme.Size<Dimension::D3D>(size);

			if(data.Scheme.IsCompressed())
				glCompressedTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, Settings.Format, dataSize, imageData);
			else
				glTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, data.PixelFormat, data.PixelType, imageData);

			imageData += dataSize;
		}

		if(Settings.MipCount != 1 && data.MipCount == 1) glGenerateTextureMipmap(ID);

		Free();
	}

	void Texture3D::CopyFrom(const GL::Texture& o)
	{
		glCopyImageSubData(o.Get(), o.GetTarget(), 0, 0, 0, 0, ID, GL_TEXTURE_3D, 0, 0, 0, 0, GetSize().x, GetSize().y, GetSize().z);
	}

	void TextureCube::Construct()
	{
		const GPU::TextureData& data = GetData();
		const u8* imageData = data.Data.Data();
		TextureSize1D size = GetSize();

		glTextureStorage2D(ID, Settings.MipCount, Settings.Format, size, size);

		if(!data.Data) return;
		for(u8 i = 0; i < data.MipCount; i++, size >>= 1)
		{
			size = glm::max(size, 1u);
			u64 dataSize = data.Scheme.Size<Dimension::D3D>(TextureSize3D(size, size, 6));

			if(data.Scheme.IsCompressed())
				glCompressedTextureSubImage3D(ID, i, 0, 0, 0, size, size, 6, Settings.Format, dataSize, imageData);
			else
				glTextureSubImage3D(ID, i, 0, 0, 0, size, size, 6, data.PixelFormat, data.PixelType, imageData);

			imageData += dataSize;
		}

		if(Settings.MipCount != 1 && data.MipCount == 1) glGenerateTextureMipmap(ID);

		Free();
	}

	void TextureCube::CopyFrom(const GL::Texture& o)
	{
		glCopyImageSubData(o.Get(), o.GetTarget(), 0, 0, 0, 0, ID, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0, GetSize(), GetSize(), 6);
	}

	u8 TextureSlotManager::Increment(const Texture& tex)
	{
		tex.Use(_index);
		return _index++;
	}
}
