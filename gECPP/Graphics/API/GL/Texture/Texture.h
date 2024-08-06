#pragma once

#include <Graphics/API/GL/GL.h>
#include "Engine/Binary/Binary.h"
#include "GLAD/glad.h"
#include "TextureSettings.h"

#define GE_ANISOTROPY_COUNT 8

// Type is more "integral" so I use a lowercase convention
// Opaque type to prevent accidental pointer-to-handle conversions
struct handle
{
	inline explicit handle() = default;
	inline explicit handle(u64 i) : ID(i) {};

	ALWAYS_INLINE handle& operator=(u64 o) { ID = o; return *this; }

	OPERATOR_CAST_CONST(u64, ID);

	u64 ID = 0;
};

namespace GL
{
	struct FrameBuffer;

	CONSTEXPR_GLOBAL handle NullHandle = handle();

	template<Dimension T>
	u8 GetMipCount(const Size<T>& size)
	{
		u32 largest;

		if constexpr(T == Dimension::D1D) largest = size;
		else if constexpr(T == Dimension::D2D) largest = glm::max(size.x, size.y);
		else largest = glm::max(size.x, glm::max(size.y, size.z));

		return 32 - __builtin_clz(largest);
	}

 	class Texture : gE::Graphics::Texture, public GLObject
	{
	 public:
		Texture(gE::Window* window, GLenum target, const ITextureSettings& settings);

		inline void Bind() const override { glBindTexture(Target, ID); }

		ALWAYS_INLINE int32_t Use(int32_t slot) const { glBindTextureUnit(slot, ID); return slot; } // NOLINT

		inline u32 Bind(u32 unit, GLenum access, u8 mip = 0, GLenum format = 0) const
		{
			glBindImageTexture(unit, ID, mip, Target == GL_TEXTURE_3D, 0, access, format ?: Settings.Format);
			return unit;
		}

		inline void Free() override { Data.Free(); }
		inline bool IsFree() override { return Data.IsFree(); }

		handle GetHandle();
		virtual void CopyFrom(const GL::Texture&) = 0;

		explicit ALWAYS_INLINE operator handle() { return GetHandle(); }

		GET_CONST(GLenum, Format, Settings.Format);
		GET_CONST(GLenum, Target, Target);
		GET_CONST(u8, MipCount, Settings.MipCount);

		~Texture() override;

	 protected:
		ITextureSettings Settings;
		TextureData Data;
		GLenum Target;

	 private:
		handle _handle = NullHandle;
	};

	class Texture2D final : public Texture
	{
	 public:
		Texture2D(gE::Window* window, const TextureSettings2D& settings, TextureData&& = {});

		NODISCARD ALWAYS_INLINE GL::TextureSize2D GetSize(u8 mip = 0) const { return glm::max(_size >> glm::u32vec2(mip), glm::u32vec2(1)); }
		void CopyFrom(const GL::Texture&) override;

	 private:
		const GL::TextureSize2D _size;
	};

	class Texture3D final : public Texture
	{
	 public:
		Texture3D(gE::Window* window, const TextureSettings3D& settings, TextureData&& = {});

		NODISCARD ALWAYS_INLINE GL::TextureSize3D GetSize(u8 mip = 0) const { return glm::max(_size >> glm::u32vec3(mip), glm::u32vec3(1)); }
		void CopyFrom(const GL::Texture&) override;

	 private:
		const GL::TextureSize3D _size;
	};

	class TextureCube final : public Texture
	{
	 public:
		TextureCube(gE::Window* window, const TextureSettings1D& settings, TextureData&& = {});

		NODISCARD ALWAYS_INLINE GL::TextureSize1D GetSize(u8 mip = 0) const { return MAX(_size >> mip, 1); }
		void CopyFrom(const GL::Texture&) override {};

	 private:
		const GL::TextureSize1D _size;
	};
}

namespace PVR
{
	NODISCARD GL::Texture* Read(gE::Window* window, const char* path, GL::WrapMode = GL::WrapMode::Repeat, GL::FilterMode = GL::FilterMode::Linear);
	NODISCARD Array<u8> Read(const char* path, PVR::Header& header);
}