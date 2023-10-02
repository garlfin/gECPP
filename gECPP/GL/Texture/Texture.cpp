#include "Texture.h"
#include <iostream>
#include <GLAD/glad.h>
#include <GL/Binary.h>
#include <GL/Buffer/FrameBuffer.h>
#include "RenderBuffer.h"

using namespace GL;

Texture::Texture(gE::Window* window, GLenum tgt, const SizelessTextureSettings& settings) :
	Asset(window), Mips(settings.MipCount), Format(settings.Format), Target(tgt)
{
	glCreateTextures(tgt, 1, &ID);
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, (GLint) settings.Filter + (Mips > 1 ? 0x102 : 0));
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, (GLint) settings.Filter);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture2D::Texture2D(gE::Window* window, const TextureSettings<TextureDimension::D2D>& settings, const TextureData& data) :
	Texture(window, GL_TEXTURE_2D, settings), _size(settings.Size)
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

			if(data.Scheme)
				glCompressedTextureSubImage2D(ID, i, 0, 0, size.x, size.y, Format, dataSize, dataPtr);
			else
				glTextureSubImage2D(ID, i, 0, 0, size.x, size.y, data.PixelFormat, data.PixelType, dataPtr);

			dataPtr += dataSize;
		}
	else glGenerateTextureMipmap(ID);
}

Texture3D::Texture3D(gE::Window* window, const TextureSettings<TextureDimension::D3D>& settings, const TextureData& data) :
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

		if(data.Scheme)
			glCompressedTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, Format, dataSize, dataPtr);
		else
			glTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, data.PixelFormat, data.PixelType, dataPtr);

		dataPtr += dataSize;
	}
}

Texture2D* PVR::Read(gE::Window* window, const char* path, WrapMode wrapMode, FilterMode filterMode)
{
	u32 fileLen = 0;
	u8* f = ReadFile(path, fileLen, false);
	if (!f) return nullptr;

	u8* ptr = f;
	PVRHeader h;
	h.Serialize(ptr);

	if(h.Depth + h.Surfaces + h.Faces > 3) std::cout << "Unexpected 3D Texture" << std::endl;

	TextureSettings<GL::TextureDimension::D2D> settings
	{
		PVRToInternalFormat(h.Format),
		wrapMode,
		filterMode,
		(u8) h.MipCount,
		h.Size,
	};

	TextureData data
	{
		GL_NONE,
		GL_NONE,
		CompressionScheme(16, 16),
		ptr,
		true
	};

	auto* tex = new Texture2D(window, settings, data);

	delete[] f;
	return tex;
}

void PVR::PVRHeader::Serialize(u8*& ptr)
{
	Version = ::Read<u32>(ptr);
	Flags = ::Read<PVRFlags>(ptr);
	Format = ::Read<PVRPixelFormat>(ptr);
	ColorSpace = ::Read<PVRColorSpace>(ptr);
	::Read<uint32_t>(ptr); // This was like bpc or something; unimportant w/ compression
	Size = ::Read<glm::u32vec2>(ptr);
	Size = { Size.y, Size.x }; // NOLINT
	// they store it height, width 🤦‍♂️
	Depth = ::Read<u32>(ptr);
	Surfaces = ::Read<u32>(ptr);
	Faces = ::Read<u32>(ptr);
	MipCount = ::Read<u32>(ptr);
	ptr += ::Read<u32>(ptr); // I couldn't give two hoots about the metadata
}

void PVR::PVRHeader::Deserialize(gETF::SerializationBuffer&) const {}

void Texture::Attach(GL::FrameBuffer* buffer, GLenum attachment, u8 mip) const
{
	glNamedFramebufferTexture(buffer->Get(), attachment, ID, mip);
}

Texture::~Texture()
{
	glDeleteTextures(1, &ID);
	if(_handle) glMakeTextureHandleNonResidentARB(_handle);
}

TextureHandle Texture::Handle()
{
	if(_handle) return _handle;

	_handle = glGetTextureHandleARB(ID);
	glMakeTextureHandleResidentARB(_handle);
}

void RenderBuffer::Attach(GL::FrameBuffer* buffer, GLenum attachment, u8 mip) const
{
	glNamedFramebufferRenderbuffer(ID, attachment, GL_RENDERBUFFER, ID);
}

TextureCubemap::TextureCubemap(gE::Window* window, const TextureSettings<TextureDimension::D1D>& settings, const TextureData& data) :
	Texture(window, GL_TEXTURE_CUBE_MAP, settings), _size(settings.Size)
{
	glTextureStorage2D(ID, Mips, settings.Format, _size, _size);

	if(!data.Data) return;

	u32 size = _size;
	u8* dataPtr = (u8*) data.Data;

	for(ubyte i = 0; i < Mips; i++, size >>= 1)
	{
		size = glm::max(size, 1u);
		u64 dataSize = data.Scheme.Size<TextureDimension::D2D>(TextureSize2D(size));

		if(data.Scheme)
			glCompressedTextureSubImage3D(ID, i, 0, 0, 0, _size, _size, 6, Format, dataSize, dataPtr);
		else
			glTextureSubImage3D(ID, i, 0, 0, 0, _size, _size, 6, data.PixelFormat, data.PixelType, dataPtr);

		dataPtr += dataSize;
	}
}
