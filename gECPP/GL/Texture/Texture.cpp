#include "Texture.h"
#include <iostream>
#include <GLAD/glad.h>
#include <GL/Binary.h>
#include <sys/stat.h>

using namespace GL;

#define DIV_CEIL(x, y) (((x) + (y) - decltype(x)(1)) / (y))

constexpr GLenum PVRToInternalFormat(PVR::PVRPixelFormat f)
{
	// TODO: Implement table instead of switch.
	switch(f)
	{
	case PVR::PVRPixelFormat::DXT1: return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
	case PVR::PVRPixelFormat::DXT3: return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	case PVR::PVRPixelFormat::DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	case PVR::PVRPixelFormat::BC5: return GL_COMPRESSED_RG_RGTC2;
	case PVR::PVRPixelFormat::Depth: return GL_DEPTH_COMPONENT16;
	default: return GL_RGB8;
	}
}

struct CompressionScheme
{
	uint8_t Pixel;
	uint8_t Byte;

	NODISCARD size_t GetByteSize(const gl::u16vec2& size) const { return DIV_CEIL(size.x * size.y, Pixel) * Byte; }
};

inline bool FormatIsCompressed(PVR::PVRPixelFormat f) { return f != PVR::PVRPixelFormat::R8G8B8; }

Texture::Texture(gE::Window* window, GLuint target, TextureSize size, uint8_t mips, bool linear) : Asset(window), Size(size), Mips(mips)
{
	glCreateTextures(target, 1, &ID);
	glTextureParameteri(ID, GL_TEXTURE_MIN_FILTER, linear ? (mips > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) : GL_NEAREST);
	glTextureParameteri(ID, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(ID, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture2D::Texture2D(gE::Window* window, TextureSize size, PVR::PVRPixelFormat f, u8* d, uint8_t mips, bool linear)
	: Texture(window, GL_TEXTURE_2D, size, mips, linear)
{
	size = TextureSize::Max(size, TextureSize(1));
	glTextureStorage2D(ID, mips, PVRToInternalFormat(f), size.x, size.y);
	uint32_t dataSize;
	if(!d) return;
	for(ubyte i = 0; i < mips; i++, size = DIV_CEIL(size, TextureSize(2)), d += dataSize) // TODO: case for format, lazy
	{
		if(FormatIsCompressed(f))
		{
			dataSize = DIV_CEIL(size.x * size.y, 16) * 16; // 16 bytes for 16 pixels
			glCompressedTextureSubImage2D(ID, i, 0, 0, size.x, size.y, PVRToInternalFormat(f), dataSize, d);
		}
		else
		{
			dataSize = size.x * size.y * 3; // 3bpp
			glTextureSubImage2D(ID, i, 0, 0, size.x, size.y, GL_RGB, GL_UNSIGNED_BYTE, d);
		}
	}
	if(!mips) glGenerateTextureMipmap(ID);
}

Texture* PVR::Read(gE::Window* window, const char* path, bool linear)
{
	u32 fileLen = 0;
	u8* f = ReadFile(path, fileLen, false);
	if (!f) return nullptr;

	u8* ptr = f;
	PVRHeader h;
	h.Serialize(ptr);

	if(h.Depth + h.Surfaces + h.Faces > 3) std::cout << "Unexpected 3D Texture" << std::endl;
	auto* tex = new Texture2D(window, h.Size, h.Format, ptr, h.MipCount, linear);
	delete[] f;

	return tex;
}

void PVR::PVRHeader::Serialize(u8*& ptr)
{
	Version = ::Read<uint32_t>(ptr);
	Flags = ::Read<PVRFlags>(ptr);
	Format = ::Read<PVRPixelFormat>(ptr);
	ColorSpace = ::Read<PVRColorSpace>(ptr);
	::Read<uint32_t>(ptr); // This was like bpc or something; unimportant w/ compression
	Size = ::Read<TextureSize>(ptr);
	Size = {Size.y, Size.x};
	// they store it height, width 🤦‍♂️
	Depth = ::Read<uint32_t>(ptr);
	Surfaces = ::Read<uint32_t>(ptr);
	Faces = ::Read<uint32_t>(ptr);
	MipCount = ::Read<uint32_t>(ptr);
	ptr += ::Read<uint32_t>(ptr); // I couldn't give two hoots about the metadata
}