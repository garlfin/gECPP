#include "Texture.h"

#include <Engine/Binary/Binary.h>
#include <GLAD/glad.h>
#include <Graphics/Texture/Texture.h>

namespace GL
{
	Texture::Texture(gE::Window* window, GLenum target, const GPU::ITextureSettings& settings) :
		APIObject(window), Settings(settings), Target(target)
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


	Texture2D::Texture2D(gE::Window* window, const GPU::TextureSettings<Dimension::D2D>& settings, GPU::TextureData&& data)
		: Texture(window, GL_TEXTURE_2D, settings), _size(settings.Size)
	{
		if(!Settings.MipCount) Settings.MipCount = GPU::GetMipCount<Dimension::D2D>(_size);
		glTextureStorage2D(ID, Settings.MipCount, Settings.Format, _size.x, _size.y);

		if(!data.Data) return;

		glm::u32vec2 size = _size;
		u8* dataPtr = data.Data.Data();

		for(u8 i = 0; i < data.MipCount; i++, size >>= TextureSize2D(1))
		{
			size = max(size, TextureSize2D(1));
			u64 dataSize = data.Scheme.Size<Dimension::D2D>(size);

			if(data.Scheme.IsCompressed())
				glCompressedTextureSubImage2D(ID, i, 0, 0, size.x, size.y, Settings.Format, dataSize, dataPtr);
			else
				glTextureSubImage2D(ID, i, 0, 0, size.x, size.y, data.PixelFormat, data.PixelType, dataPtr);

			dataPtr += dataSize;
		}

		if(Settings.MipCount != 1 && data.MipCount == 1) glGenerateTextureMipmap(ID);
	}

	void Texture2D::CopyFrom(const Texture& o)
	{
		glCopyImageSubData(o.Get(), GL_TEXTURE_2D, 0, 0, 0, 0, ID, GL_TEXTURE_2D, 0, 0, 0, 0, GetSize().x, GetSize().y, 1);
	}

	Texture3D::Texture3D(gE::Window* window, const GPU::TextureSettings<Dimension::D3D>& settings, GPU::TextureData&& data)
		: Texture(window, GL_TEXTURE_3D, settings), _size(settings.Size)
	{
		if(!Settings.MipCount) Settings.MipCount = GPU::GetMipCount<Dimension::D3D>(_size);
		glTextureStorage3D(ID, Settings.MipCount, Settings.Format, _size.x, _size.y, _size.z);

		if(!data) return;

		glm::u32vec3 size = _size;
		u8* dataPtr = data.Data.Data();

		for(u8 i = 0; i < data.MipCount; i++, size >>= TextureSize3D(1))
		{
			size = max(size, TextureSize3D(1));
			u64 dataSize = data.Scheme.Size<Dimension::D3D>(size);

			if(data.Scheme.IsCompressed())
				glCompressedTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, Settings.Format, dataSize, dataPtr);
			else
				glTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, data.PixelFormat, data.PixelType, dataPtr);

			dataPtr += dataSize;
		}

		if(Settings.MipCount != 1 && data.MipCount == 1) glGenerateTextureMipmap(ID);
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

	TextureCube::TextureCube(gE::Window* window, const GPU::TextureSettings<Dimension::D1D>& settings, GPU::TextureData&& data)
		: Texture(window, GL_TEXTURE_CUBE_MAP, settings), _size(settings.Size)
	{
		if(!Settings.MipCount) Settings.MipCount = GPU::GetMipCount<Dimension::D1D>(_size);
		glTextureStorage2D(ID, Settings.MipCount, settings.Format, _size, _size);

		if(!data.Data) return;

		u32 size = _size;
		u8* dataPtr = data.Data.Data(); // sobbing rn

		for(u8 i = 0; i < data.MipCount; i++, size >>= 1)
		{
			size = glm::max(size, 1u);
			u64 dataSize = data.Scheme.Size<Dimension::D3D>(TextureSize3D(size, size, 6));

			if(data.Scheme.IsCompressed())
				glCompressedTextureSubImage3D(ID, i, 0, 0, 0, size, size, 6, settings.Format, dataSize, dataPtr);
			else
				glTextureSubImage3D(ID, i, 0, 0, 0, size, size, 6, data.PixelFormat, data.PixelType, dataPtr);

			dataPtr += dataSize;
		}

		if(Settings.MipCount != 1 && data.MipCount == 1) glGenerateTextureMipmap(ID);
	}
}
