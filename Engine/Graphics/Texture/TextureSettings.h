//
// Created by scion on 8/5/2024.
//

#pragma once

#include <Core/Binary.h>
#include <Core/Math/Math.h>
#include <Core/Serializable/Serializable.h>
#include <GLAD/glad.h>

namespace GPU
{
	enum class FilterMode : GLenum
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR
	};

	CONSTEXPR_GLOBAL EnumData<FilterMode, 2> EFilterMode
	{
		EnumType::Normal,
		{
			ENUM_DEF(FilterMode, Nearest),
			ENUM_DEF(FilterMode, Linear)
		}
	};

	enum class WrapMode : GLenum
	{
		Clamp = GL_CLAMP_TO_EDGE,
		Repeat = GL_REPEAT,
		Border = GL_CLAMP_TO_BORDER
	};

	CONSTEXPR_GLOBAL EnumData<WrapMode, 3> EWrapMode
	{
		EnumType::Normal,
		{
			ENUM_DEF(WrapMode, Clamp),
			ENUM_DEF(WrapMode, Repeat),
			ENUM_DEF(WrapMode, Border)
		}
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

	struct TextureData : public Serializable<>
	{
		SERIALIZABLE_PROTO("TEXD", 1, TextureData, Serializable);

	public:
		TextureData(GLenum, GLenum, CompressionScheme, u8, Array<u8>&&);

		GLenum PixelFormat = GL_RGB;
		GLenum PixelType = GL_UNSIGNED_BYTE;
		CompressionScheme Scheme = { 1, 1 };
		u8 MipCount = 0;

		Array<u8> Data = DEFAULT;

		NODISCARD ALWAYS_INLINE bool IsFree() const { return Data.IsFree(); }
		ALWAYS_INLINE void Free() { Data.Free(); }
		ALWAYS_INLINE operator bool() const { return Data.Data(); }
	};

	bool FormatIsCompressed(GLenum f);
}