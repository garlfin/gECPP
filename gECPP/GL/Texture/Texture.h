#pragma once

#include <GL/gl.h>
#include <gEModel/gETF/Prototype.h>
#include <GL/Binary.h>
#include <GLAD/glad.h>

#define DIV_CEIL(x, y) (((x) + (y) - decltype(x)(1)) / (y))

namespace GL { typedef glm::u32vec2 TextureSize; typedef glm::u32vec3 TextureSize3D; }

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
}

namespace GL
{
	struct CompressionScheme
	{
		u8 BlockSize;
		u8 ByteSize;

		template<typename DIMENSION>
		NODISCARD ALWAYS_INLINE u64 Size(const DIMENSION& size)
		{
			DIMENSION blocks = DIV_CEIL(size, BlockSize);
			if constexpr(std::is_same_v<DIMENSION, TextureSize>) return blocks.x * blocks.y * ByteSize;
			else return blocks.x * blocks.y * blocks.z * ByteSize;
		}
	};

	template<typename DIMENSION>
	struct TextureSettings
	{
		DIMENSION Size = DIMENSION(1);
		u8 Mips = 0;
		GLenum Format = GL_UNSIGNED_BYTE;
		GLenum WrapMode = GL_REPEAT;
		GLenum Filter = GL_LINEAR;
	};

	template<typename DIMENSION>
	class Texture : public Asset
	{
	 public:
		NODISCARD ALWAYS_INLINE uint32_t Use(uint32_t slot) const { glBindTextureUnit(slot, ID); return slot; }
		NODISCARD ALWAYS_INLINE TextureSize GetSize(u8 mip = 0) const { return Size >> DIMENSION(mip); }
		ALWAYS_INLINE u32 Bind(u32 unit, GLenum access, u8 mip = 0) { glBindImageTexture(unit, ID, mip, GL_FALSE, 0, access, Format); return unit; }

		inline void Bind() const override  { glBindTexture(Target, ID); }
		~Texture() override { glDeleteTextures(1, &ID); }

	 protected:
		Texture(gE::Window* window, GLuint target, GLenum Format, const DIMENSION& size, u8, bool linear = true);

		const DIMENSION Size;
		const uint8_t Mips;
		const GLenum Format;
		const GLenum Target;
	};

	template class Texture<TextureSize>;
	template class Texture<TextureSize3D>;

	class Texture2D final : public Texture<TextureSize>
	{
	 public:
		Texture2D(gE::Window* window, TextureSize, PVR::PVRPixelFormat, u8* d = nullptr, u8 mips = 0, bool linear = true);
	};

	class Texture3D final : public Texture<TextureSize3D>
	{
	 public:
		Texture3D(gE::Window*, const TextureSize3D&, GLenum fmt, u8* = nullptr, u8* mip = 0, bool linear = true);

		NODISCARD ALWAYS_INLINE TextureSize3D GetSize3D(u8 mip = 0) { return _size >> TextureSize3D(mip); }

	 private:
		TextureSize3D _size;
	};
}

namespace PVR
{
	GL::Texture<GL::TextureSize>* Read(gE::Window* window, const char* path, bool linear = true);
}