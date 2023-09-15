#include "Texture.h"
#include <iostream>
#include <GLAD/glad.h>
#include <GL/Binary.h>
#include <sys/stat.h>

using namespace GL;

template<TextureDimension DIMENSION>
Texture<DIMENSION>::Texture(gE::Window* window, GLenum tgt, const TextureSettings<DIMENSION>& settings) :
	Asset(window), Size(settings.Size), Mips(settings.MipCount), Format(settings.Format), Target(tgt)
{
	glCreateTextures(tgt, 1, &ID);
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, (GLint) settings.Filter + (Mips > 1 ? 0x102 : 0));
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, (GLint) settings.Filter);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture2D::Texture2D(gE::Window* window, const TextureSettings<TextureDimension::D2D>& settings, const TextureData& data) :
	Texture<TextureDimension::D2D>(window, GL_TEXTURE_2D, settings)
{
	glTextureStorage2D(ID, Mips, Format, Size.x, Size.y);

	if(!data.Data) return;

	TextureSize<TextureDimension::D2D> size = Size;
	u8* dataPtr = (u8*) data.Data;
	bool isCompressed = settings.Scheme.BlockSize != 1;

	for(ubyte i = 0; i < Mips; i++, size >>= decltype(size)(1)) // lazy guy
	{
		size = glm::max(size, decltype(size)(1)); // double lazy guy
		u64 dataSize = settings.Scheme.Size<TextureDimension::D2D>(size);

		if(isCompressed) glCompressedTextureSubImage2D(ID, i, 0, 0, size.x, size.y, Format, dataSize, dataPtr);
		else glTextureSubImage2D(ID, i, 0, 0, size.x, size.y, GL_RGB, GL_UNSIGNED_BYTE, dataPtr);

		dataPtr += dataSize;
	}
}

Texture3D::Texture3D(gE::Window* window, const TextureSettings<TextureDimension::D3D>& settings, const TextureData& data) :
	Texture<TextureDimension::D3D>(window, GL_TEXTURE_3D, settings)
{
	glTextureStorage3D(ID, Mips, Format, Size.x, Size.y, Size.z);

	if(!data.Data) return;

	TextureSize<TextureDimension::D3D> size = Size;
	u8* dataPtr = (u8*) data.Data;

	for(ubyte i = 0; i < Mips; i++, size >>= decltype(size)(1)) // lazy guy
	{
		size = glm::max(size, decltype(size)(1)); // double lazy guy
		u64 dataSize = settings.Scheme.Size<TextureDimension::D2D>(size);

		glTextureSubImage3D(ID, i, 0, 0, 0, size.x, size.y, size.z, GL_RGB, GL_UNSIGNED_BYTE, dataPtr); // TODO: ADD CASES!!!

		dataPtr += dataSize;
	}
}

Texture<GL::TextureDimension::D2D>* PVR::Read(gE::Window* window, const char* path, WrapMode wM, FilterMode fM)
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
		h.Size,
		PVRToInternalFormat(h.Format),
		CompressionScheme(16, 16),
		wM,
		fM,
		(u8) h.MipCount
	};

	auto* tex = new Texture2D(window, settings, { ptr, (u8) h.MipCount });

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
	Size = { Size.y, Size.x };
	// they store it height, width 🤦‍♂️
	Depth = ::Read<u32>(ptr);
	Surfaces = ::Read<u32>(ptr);
	Faces = ::Read<u32>(ptr);
	MipCount = ::Read<u32>(ptr);
	ptr += ::Read<u32>(ptr); // I couldn't give two hoots about the metadata
}

void PVR::PVRHeader::Deserialize(gETF::SerializationBuffer&) const {}

TextureHandle::TextureHandle(const Texture<TextureDimension::D2D>& tex) :
	_id(tex.Get()), _format(tex.GetFormat()), _target(tex.GetTarget())
{
}

TextureHandle::TextureHandle(const Texture<TextureDimension::D3D>& tex) :
	_id(tex.Get()), _format(tex.GetFormat()), _target(tex.GetTarget())
{
}

bool FormatIsCompressed(GLenum f)
{
	switch(f)
	{
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	case GL_COMPRESSED_RG_RGTC2:
		return true;
	default:
		return false;
	}
}
