//
// Created by scion on 9/13/2023.
//

#pragma once

#include <GL/Math.h>
#include <gEModel/gETF/Prototype.h>
#include "GLAD/glad.h"
#include "GL/Binary.h"

#define DIV_CEIL(X, Y) (((X) + (Y) - decltype(X)(1)) / (Y))
#define DIV_CEIL_T(X, Y, T) (((T)(X) + (T)(Y) - (T)(1)) / (T)(Y))

namespace PVR
{
	enum class PVRFlags : u32
	{
		None = 0,
		PreMultiplied = 2
	};

	enum class PVRPixelFormat : u64
	{
		DXT1 = 7,
		DXT3 = 9,
		DXT5 = 11,
		R8G8B8 = 2260630272894834, // rgb.888.
		Depth, BC5 = 13
	};

	enum class PVRColorSpace : u32
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
		glm::u32vec2 Size;
		uint32_t Depth;
		uint32_t Surfaces;
		uint32_t Faces;
		uint32_t MipCount;
	};

	constexpr GLenum PVRToInternalFormat(PVR::PVRPixelFormat f)
	{
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
		D2D,
		D3D
	};

	template<TextureDimension DIMENSION = TextureDimension::D2D>
	using TextureSize = std::conditional_t<DIMENSION == TextureDimension::D2D, glm::u32vec2, glm::u32vec3>;

	struct CompressionScheme
	{
		u8 BlockSize;
		u8 ByteSize;

		template<TextureDimension DIMENSION>
		NODISCARD ALWAYS_INLINE u64 Size(const TextureSize<DIMENSION>& size) const
		{
			TextureSize<DIMENSION> blocks = DIV_CEIL_T(size, BlockSize, TextureSize<DIMENSION>);
			if constexpr(DIMENSION == TextureDimension::D2D) return blocks.x * blocks.y * ByteSize;
			else return blocks.x * blocks.y * blocks.z * ByteSize;
		}
	};

	template<TextureDimension DIMENSION>
	struct TextureSettings
	{
		TextureSize<DIMENSION> Size = TextureSize<DIMENSION>(1);
		GLenum Format = GL_UNSIGNED_BYTE;
		CompressionScheme Scheme = {1, 1};
		WrapMode WrapMode = WrapMode::Repeat;
		FilterMode Filter = FilterMode::Linear;
		u8 MipCount = 0;
	};

	struct TextureData
	{
		void* Data = nullptr;
		u8 SentMips = 0;
	};

	bool FormatIsCompressed(GLenum f);
}
