#pragma once

#include <GL/gl.h>
#include <gEModel/gETF/Prototype.h>
#include <GL/Binary.h>
#include <GLAD/glad.h>

namespace GL { class Texture; typedef gl::u32vec2 TextureSize; }

namespace PVR
{
	enum class PVRFlags : uint32_t
	{
		None = 0,
		PreMultiplied = 2
	};

	enum class PVRPixelFormat : uint64_t
	{
		DXT1 = 7,
		DXT3 = 9,
		DXT5 = 11,
		R8G8B8 = 2260630272894834, // rgb.888.
		Depth, BC5 = 13
	};

	enum class PVRColorSpace : uint32_t
	{
		Linear = 0,
		SRGB = 1,
	};

	struct PVRHeader : gETF::Serializable
	{
		SERIALIZABLE_PROTO;

		uint32_t Version;
		PVRFlags Flags;
		PVRPixelFormat Format;
		PVRColorSpace ColorSpace;
		GL::TextureSize Size;
		uint32_t Depth;
		uint32_t Surfaces;
		uint32_t Faces;
		uint32_t MipCount;
	};

	enum class FilterMode : GLenum
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR
	};

	GL::Texture* Read(gE::Window* window, const char* path, bool linear = true);
}

namespace GL
{
	class Texture : public Asset
	{
	 public:
		NODISCARD ALWAYS_INLINE uint32_t Use(uint32_t slot) const { glBindTextureUnit(slot, ID); return slot; }
		NODISCARD ALWAYS_INLINE TextureSize GetSize(u8 mip = 0) const { return TextureSize(Size.x >> mip, Size.y >> mip); }

		~Texture() override { glDeleteTextures(1, &ID); }

	 protected:
		Texture(gE::Window* window, GLuint target, TextureSize, u8, bool linear = true);

		const TextureSize Size;
		const uint8_t Mips;
	};

	class Texture2D final : public Texture
	{
	 public:
		Texture2D(gE::Window* window, TextureSize, PVR::PVRPixelFormat, u8* d, u8 mips = 0, bool linear = true);

		ALWAYS_INLINE void Bind() const override { glBindTexture(GL_TEXTURE_2D, ID); }
	};
}