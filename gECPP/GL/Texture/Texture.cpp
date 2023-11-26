#include "Texture.h"
#include <iostream>
#include <GLAD/glad.h>
#include "GL/Binary/Binary.h"
#include <GL/Buffer/FrameBuffer.h>
#include "RenderBuffer.h"
#include "TextureSlotManager.h"

namespace GL
{
	Texture::Texture(gE::Window* window, GLenum tgt, const SizelessTextureSettings& settings) :
		Asset(window), Mips(settings.MipCount), Format(settings.Format), Target(tgt)
	{
		glCreateTextures(tgt, 1, &ID);
		glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, (GLint) settings.Filter + (Mips > 1 ? 0x102 : 0));
		glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, (GLint) settings.Filter);
		glTextureParameteri(ID, GL_TEXTURE_WRAP_S, (GLint) settings.WrapMode);
		glTextureParameteri(ID, GL_TEXTURE_WRAP_T, (GLint) settings.WrapMode);
	}

	Texture2D::Texture2D(gE::Window* window, const TextureSettings<TextureDimension::D2D>& settings, const TextureData& data)
		: Texture(window, GL_TEXTURE_2D, settings), _size(settings.Size)
	{
		glTextureStorage2D(ID, Mips, Format, _size.x, _size.y);

		if(!data.Data) return;

		glm::u32vec2 size = _size;
		u8* dataPtr = (u8*) data.Data;

		if(data.SentAllMips)
			for(ubyte i = 0; i < Mips; i++, size >>= decltype(size)(1)) // lazy guy
			{
				size = glm::max(size, decltype(size)(1)); // double lazy guy
				u64 dataSize = data.Scheme.Size<TextureDimension::D2D>(size);

				if(data.Scheme.IsCompressed())
					glCompressedTextureSubImage2D(ID, i, 0, 0, size.x, size.y, Format, dataSize, dataPtr);
				else
					glTextureSubImage2D(ID, i, 0, 0, size.x, size.y, data.PixelFormat, data.PixelType, dataPtr);

				dataPtr += dataSize;
			}
		else
			glGenerateTextureMipmap(ID);
	}

	void Texture2D::CopyFrom(const Texture& o)
	{
		glCopyImageSubData(o.Get(), GL_TEXTURE_2D, 0, 0, 0, 0, ID, GL_TEXTURE_2D, 0, 0, 0, 0, GetSize().x, GetSize().y, 1);
	}

	Texture3D::Texture3D(gE::Window* window, const TextureSettings<TextureDimension::D3D>& settings, const TextureData& data)
		:
		Texture(window, GL_TEXTURE_3D, settings), _size(settings.Size)
	{
		glTextureStorage3D(ID, Mips, Format, _size.x, _size.y, _size.z);

		if(!data.Data) return;

		glm::u32vec3 size = _size;
		u8* dataPtr = (u8*) data.Data;

		for(ubyte i = 0; i < Mips; i++, size >>= decltype(size)(1)) // lazy guy
		{
			size = glm::max(size, decltype(size)(1)); // double lazy guy
			u64 dataSize = data.Scheme.Size<TextureDimension::D3D>(size);

			if(data.Scheme.IsCompressed())
				glCompressedTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, Format, dataSize, dataPtr);
			else
				glTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, data.PixelFormat, data.PixelType, dataPtr);

			dataPtr += dataSize;
		}
	}

	void Texture::Attach(GL::FrameBuffer* buffer, GLenum attachment, u8 mip) const
	{
		glNamedFramebufferTexture(buffer->Get(), attachment, ID, mip);
	}

	Texture::~Texture()
	{
		if(_handle > 1) glMakeTextureHandleNonResidentARB(_handle);
		glDeleteTextures(1, &ID);
	}

	TextureHandle Texture::GetHandle()
	{
		if(_handle) return _handle;
		if(!GLAD_GL_ARB_bindless_texture) return _handle = 1;

		_handle = glGetTextureHandleARB(ID);
		glMakeTextureHandleResidentARB(_handle);

		LOG("New handle: " << _handle);

		return _handle;
	}

	void RenderBuffer::Attach(GL::FrameBuffer* buffer, GLenum attachment, u8 mip) const
	{
		glNamedFramebufferRenderbuffer(ID, attachment, GL_RENDERBUFFER, ID);
	}

	TextureCube::TextureCube(gE::Window* window, const TextureSettings<TextureDimension::D1D>& settings, const TextureData& data)
		: Texture(window, GL_TEXTURE_CUBE_MAP, settings), _size(settings.Size)
	{
		glTextureStorage2D(ID, Mips, settings.Format, _size, _size);

		if(!data.Data) return;

		u32 size = _size;
		u8* dataPtr = (u8*) data.Data;

		for(ubyte i = 0; i < Mips; i++, size >>= 1)
		{
			size = glm::max(size, 1u);
			u64 dataSize = data.Scheme.Size<TextureDimension::D3D>(TextureSize3D(size, size, 6));

			if(data.Scheme.IsCompressed())
				glCompressedTextureSubImage3D(ID, i, 0, 0, 0, size, size, 6, Format, dataSize, dataPtr);
			else
				glTextureSubImage3D(ID, i, 0, 0, 0, size, size, 6, data.PixelFormat, data.PixelType, dataPtr);

			dataPtr += dataSize;
		}
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
}