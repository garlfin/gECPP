//
// Created by scion on 8/5/2024.
//

#pragma once

//
// Created by scion on 9/13/2023.
//

#pragma once

#include <Engine/Utility/Binary.h>
#include <Engine/Math/Math.h>
#include <gETF/Serializable.h>
#include <GLAD/glad.h>

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

	struct Header : Serializable<>
	{
	public:
		explicit Header(istream& in, SETTINGS_T s) : Serializable(in, s) { ISerialize(in, s); }
		Header() = default;
		inline void Serialize(istream& in, SETTINGS_T s) override { SAFE_CONSTRUCT(*this, Header, in, s); }
		inline void Deserialize(ostream& out) const override { IDeserialize(out); }

		uint32_t Version;
		Flags Flags;
		PixelFormat Format;
		ColorSpace ColorSpace;
		glm::u32vec2 Size;
		uint32_t Depth;
		uint32_t Surfaces;
		uint32_t Faces;
		uint32_t MipCount;

	private:
		void ISerialize(istream& in, SETTINGS_T s);
		void IDeserialize(ostream& out) const;
	};

	constexpr GLenum PVRToInternalFormat(PixelFormat f)
	{
		switch(f)
		{
			case PixelFormat::DXT1: return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			case PixelFormat::DXT3: return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			case PixelFormat::DXT5: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			case PixelFormat::BC5: return GL_COMPRESSED_RG_RGTC2;
			case PixelFormat::Depth: return GL_DEPTH_COMPONENT16;
			case PixelFormat::RGB32F: return GL_RGB32F;
			case PixelFormat::RGB16F: return GL_RGB16F;
			default: return GL_RGB8;
		}
	}
}

namespace GPU
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

	struct CompressionScheme
	{
		CompressionScheme(u8 bls, u8 bs) : BlockSize(bls), ByteSize(bs) { }
		CompressionScheme() = default;

		u8 BlockSize = 1;
		u8 ByteSize = 1;

		template<Dimension DIMENSION>
		NODISCARD ALWAYS_INLINE u64 Size(const Size<DIMENSION>& size) const
		{
			::Size<DIMENSION> blocks = DIV_CEIL_T(size, BlockSize, ::Size<DIMENSION>);
			if constexpr(DIMENSION == Dimension::D1D) return blocks * ByteSize;
			else if constexpr(DIMENSION == Dimension::D2D) return blocks.x * blocks.y * ByteSize;
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

		constexpr operator bool() const { return (bool) Format; } // NOLINT
	};

	template<Dimension DIMENSION>
	struct TextureSettings : public ITextureSettings
	{
		Size<DIMENSION> Size = Size<DIMENSION>(1);
	};

	typedef TextureSettings<Dimension::D1D> TextureSettings1D;
	typedef TextureSettings<Dimension::D2D> TextureSettings2D;
	typedef TextureSettings<Dimension::D3D> TextureSettings3D;

	struct TextureData : public Serializable<>
	{
		SERIALIZABLE_PROTO(TEXD, 1, TextureData, Serializable);

	public:
		TextureData(GLenum, GLenum, CompressionScheme, u8, Array<u8>&&);

		GLenum PixelFormat = GL_RGB;
		GLenum PixelType = GL_UNSIGNED_BYTE;
		CompressionScheme Scheme = { 1, 1 };
		u8 MipCount = 0;

		Array<u8> Data;

		NODISCARD ALWAYS_INLINE bool IsFree() const { return Data.IsFree(); }
		ALWAYS_INLINE void Free() { Data.Free(); }
		ALWAYS_INLINE operator bool() const { return Data.Data(); }
	};

	bool FormatIsCompressed(GLenum f);
}
