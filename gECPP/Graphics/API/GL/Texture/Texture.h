#pragma once

#include <Graphics/API/GL/GL.h>
#include "Engine/Binary/Binary.h"
#include "GLAD/glad.h"
#include "Graphics/Texture/TextureSettings.h"

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

 	class Texture : public GLObject
	{
	 public:
		Texture(gE::Window* window, GLenum target, const GPU::ITextureSettings& settings);

		inline void Bind() const override { glBindTexture(Target, ID); }

		ALWAYS_INLINE int32_t Use(int32_t slot) const { glBindTextureUnit(slot, ID); return slot; } // NOLINT

		inline u32 Bind(u32 unit, GLenum access, u8 mip = 0, GLenum format = 0) const
		{
			glBindImageTexture(unit, ID, mip, Target == GL_TEXTURE_3D, 0, access, format ?: Settings.Format);
			return unit;
		}

		handle GetHandle();
		virtual void CopyFrom(const Texture&) = 0;

		explicit ALWAYS_INLINE operator handle() { return GetHandle(); }

		GET_CONST(GLenum, Format, Settings.Format);
		GET_CONST(GLenum, Target, Target);
		GET_CONST(u8, MipCount, Settings.MipCount);

		~Texture() override;

	 protected:
		GPU::ITextureSettings Settings;
		GPU::TextureData Data;
		GLenum Target;

	 private:
		handle _handle = NullHandle;
	};

	class Texture2D final : public Texture
	{
	 public:
		Texture2D(gE::Window* window, const GPU::TextureSettings2D& settings, GPU::TextureData&& = {});

		NODISCARD ALWAYS_INLINE TextureSize2D GetSize(u8 mip = 0) const { return glm::max(_size >> glm::u32vec2(mip), glm::u32vec2(1)); }
		void CopyFrom(const Texture&) override;

	 private:
		const TextureSize2D _size;
	};

	class Texture3D final : public Texture
	{
	 public:
		Texture3D(gE::Window* window, const GPU::TextureSettings3D& settings, GPU::TextureData&& = {});

		NODISCARD ALWAYS_INLINE TextureSize3D GetSize(u8 mip = 0) const { return glm::max(_size >> glm::u32vec3(mip), glm::u32vec3(1)); }
		void CopyFrom(const Texture&) override;

	 private:
		const TextureSize3D _size;
	};

	class TextureCube final : public Texture
	{
	 public:
		TextureCube(gE::Window* window, const GPU::TextureSettings1D& settings, GPU::TextureData&& = {});

		NODISCARD ALWAYS_INLINE TextureSize1D GetSize(u8 mip = 0) const { return MAX(_size >> mip, 1); }
		void CopyFrom(const Texture&) override {};

	 private:
		const TextureSize1D _size;
	};
}

namespace PVR
{
	NODISCARD GL::Texture* Read(gE::Window* window, const char* path, GPU::WrapMode = GPU::WrapMode::Repeat, GPU::FilterMode = GPU::FilterMode::Linear);
	NODISCARD Array<u8> Read(const char* path, Header& header);
}