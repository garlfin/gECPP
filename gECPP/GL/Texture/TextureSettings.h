//
// Created by scion on 9/13/2023.
//

#pragma once

#include <GL/Math.h>
#include <gEModel/gETF/Prototype.h>
#include "GLAD/glad.h"
#include "GL/Binary/Binary.h"

#define DIV_CEIL(X, Y) (((X) + (Y) - decltype(X)(1)) / (Y))
#define DIV_CEIL_T(X, Y, T) (((T)(X) + (T)(Y) - (T)(1)) / (T)(Y))

namespace PVR
{
	enum class Flags : u32
	{
		None = 0,
		PreMultiplied = 2
	};

	enum class PixelFormat : u64
	{
		DXT1 = 7,
		DXT3 = 9,
		DXT5 = 11,
		BC5 = 13,
		Depth,
		RBB8 = 2260630272894834,
		RGBA8 = 7267626108080808,
	};

	enum class ColorSpace : u32
	{
		Linear = 0,
		SRGB = 1,
	};

	struct Header : gETF::Serializable
	{
		SERIALIZABLE_PROTO;

		uint32_t Version;
		Flags Flags;
		PixelFormat Format;
		ColorSpace ColorSpace;
		glm::u32vec2 Size;
		uint32_t Depth;
		uint32_t Surfaces;
		uint32_t Faces;
		uint32_t MipCount;
	};

	constexpr GLenum PVRToInternalFormat(PVR::PixelFormat f)
	{

		switch(f)
		{
		case PVR::PixelFormat::DXT1: return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		case PVR::PixelFormat::DXT3: return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		case PVR::PixelFormat::DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		case PVR::PixelFormat::BC5: return GL_COMPRESSED_RG_RGTC2;
		case PVR::PixelFormat::Depth: return GL_DEPTH_COMPONENT16;
		default: return GL_RGB8;
		}
	}
}

namespace GL
{
	enum class FilterMode : GLenum
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR
	};

	enum class WrapMode : GLenum
	{
		Clamp = GL_CLAMP_TO_EDGE,
		Repeat = GL_REPEAT
	};

	enum class TextureDimension : u8
	{
		D1D,
		D2D,
		D3D
	};

	template<TextureDimension DIMENSION>
	using TextureSize = std::conditional_t<DIMENSION == TextureDimension::D1D, u32,
		std::conditional_t<DIMENSION == TextureDimension::D2D, glm::u32vec2, glm::u32vec3>>;

	struct CompressionScheme
	{
		CompressionScheme(u8 bls, u8 bs) : BlockSize(bls), ByteSize(bs)
		{ }

		u8 BlockSize;
		u8 ByteSize;

		template<TextureDimension DIMENSION>
		NODISCARD ALWAYS_INLINE u64 Size(const TextureSize<DIMENSION>& size) const
		{

			TextureSize<DIMENSION> blocks = DIV_CEIL_T(size, BlockSize, TextureSize<DIMENSION>);
			if constexpr(DIMENSION == TextureDimension::D1D) return blocks * ByteSize;
			else if constexpr(DIMENSION == TextureDimension::D2D) return blocks.x * blocks.y * ByteSize;
			else return blocks.x * blocks.y * blocks.z * ByteSize;
		}

		NODISCARD ALWAYS_INLINE constexpr operator bool() const
		{ return BlockSize != 1; } // NOLINT

		static const CompressionScheme& None()
		{

			static const CompressionScheme none{ 1, 1 };
			return none;
		}
	};

	struct SizelessTextureSettings
	{
		GLenum Format = GL_NONE;
		WrapMode WrapMode = WrapMode::Repeat;
		FilterMode Filter = FilterMode::Linear;
		u8 MipCount = 1;

		constexpr operator bool() const
		{ return (bool) Format; } // NOLINT
	};

	template<TextureDimension DIMENSION>
	struct TextureSettings : public SizelessTextureSettings
	{
		TextureSize<DIMENSION> Size = TextureSize<DIMENSION>(1);
	};

	struct TextureData
	{
		GLenum PixelFormat = GL_RGB;
		GLenum PixelType = GL_UNSIGNED_BYTE;
		CompressionScheme Scheme = { 1, 1 };

		void* Data = nullptr;
		bool SentAllMips = false;
	};

	bool FormatIsCompressed(GLenum f);
}
