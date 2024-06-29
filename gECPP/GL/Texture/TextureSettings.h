//
// Created by scion on 9/13/2023.
//

#pragma once

#include <GL/Math.h>
#include <GLAD/glad.h>
#include <Engine/Binary/Binary.h>
#include <gETF/Serializable.h>

namespace PVR
{
	enum class Flags : u32
	{
		None = 0,
		PreMultiplied = 2
	};

	// TODO: This is actually wrong!
	enum class PixelFormat : u64
	{
		DXT1 = 7,
		DXT3 = 9,
		DXT5 = 11,
		BC5 = 13,
		Depth,
		RGB8 = 2260630272894834,
		RGBA8 = 7267626108080808,
		RGB32F = 9042521072232306,
		RGB16F = 4521260539340658
	};

	enum class ColorSpace : u32
	{
		Linear = 0,
		SRGB = 1,
	};

	struct Header : Serializable<void>
	{
		SERIALIZABLE_PROTO(Header, Serializable<void>);

	 public:
		Header() = default;

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
		case PVR::PixelFormat::RGB32F: return GL_RGB32F;
		case PVR::PixelFormat::RGB16F: return GL_RGB16F;
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
		Repeat = GL_REPEAT,
		Border = GL_CLAMP_TO_BORDER
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
		CompressionScheme(u8 bls, u8 bs) : BlockSize(bls), ByteSize(bs) { }
		CompressionScheme() = default;

		u8 BlockSize = 1;
		u8 ByteSize = 1;

		template<TextureDimension DIMENSION>
		NODISCARD ALWAYS_INLINE u64 Size(const TextureSize<DIMENSION>& size) const
		{
			TextureSize<DIMENSION> blocks = DIV_CEIL_T(size, BlockSize, TextureSize<DIMENSION>);
			if constexpr(DIMENSION == TextureDimension::D1D) return blocks * ByteSize;
			else if constexpr(DIMENSION == TextureDimension::D2D) return blocks.x * blocks.y * ByteSize;
			else return blocks.x * blocks.y * blocks.z * ByteSize;
		}

		NODISCARD ALWAYS_INLINE bool IsCompressed() const { return BlockSize != 1; }
		NODISCARD ALWAYS_INLINE explicit operator bool() const { return BlockSize != 1; }
	};

	struct ITextureSettings
	{
		GLenum Format = GL_NONE;
		WrapMode WrapMode = WrapMode::Repeat;
		FilterMode Filter = FilterMode::Linear;
		u8 MipCount = 1;
		u8 MipBias = 0;

		constexpr operator bool() const { return (bool) Format; } // NOLINT
	};

	template<TextureDimension DIMENSION>
	struct TextureSettings : public ITextureSettings
	{
		TextureSize<DIMENSION> Size = TextureSize<DIMENSION>(1);
	};

	typedef TextureSettings<TextureDimension::D1D> TextureSettings1D;
	typedef TextureSettings<TextureDimension::D2D> TextureSettings2D;
	typedef TextureSettings<TextureDimension::D3D> TextureSettings3D;

	struct TextureData : public Serializable<void>
	{
		SERIALIZABLE_PROTO(TextureData, Serializable);
		TextureData(GLenum, GLenum, CompressionScheme, u8, Array<u8>&&);
		TextureData() = default;

		GLenum PixelFormat = GL_RGB;
		GLenum PixelType = GL_UNSIGNED_BYTE;
		CompressionScheme Scheme = { 1, 1 };
		u8 MipCount = 0;

		Array<u8> Data;
	};

	bool FormatIsCompressed(GLenum f);
}