#pragma once

#include <GL/GL.h>
#include <gEModel/gETF/Prototype.h>
#include "GL/Binary/Binary.h"
#include <GLAD/glad.h>
#include "TextureSettings.h"
#include "Attachment.h"

namespace GL
{
	typedef u64 TextureHandle;

	class Texture : public Asset, public Attachment
	{
	 public:
		Texture(gE::Window* window, GLenum tgt, const SizelessTextureSettings& settings);

		inline void Bind() const override { glBindTexture(Target, ID); }
		ALWAYS_INLINE uint32_t Use(uint32_t slot) const { glBindTextureUnit(slot, ID); return slot; } // NOLINT
		ALWAYS_INLINE u32 Bind(u32 unit, GLenum access, u8 mip = 0) const { glBindImageTexture(unit, ID, mip, GL_FALSE, 0, access, Format); return unit; } // NOLINT
		void Attach(GL::FrameBuffer* buffer, GLenum attachment, u8 mip) const override;

		TextureHandle GetHandle();
		explicit ALWAYS_INLINE operator TextureHandle() const { return _handle; }

		GET_CONST_VALUE(GLenum, Format, Format);
		GET_CONST_VALUE(GLenum, Target, Target);
		GET_CONST_VALUE(GLenum, MipCount, Mips);

		~Texture() override;

	 protected:
		const uint8_t Mips;
		const GLenum Format;
		const GLenum Target;

	 private:
		TextureHandle _handle = 0;
	};

	class Texture2D final : public Texture
	{
	 public:
		Texture2D(gE::Window* window, const TextureSettings<TextureDimension::D2D>& settings, const TextureData& = {});
		NODISCARD ALWAYS_INLINE GL::TextureSize2D GetSize(u8 mip = 0) const { return _size >> glm::u32vec2(mip); }

	 private:
		const GL::TextureSize2D _size;
	};

	class Texture3D final : public Texture
	{
	 public:
		Texture3D(gE::Window* window, const TextureSettings<TextureDimension::D3D>& settings, const TextureData& = {});
		NODISCARD ALWAYS_INLINE GL::TextureSize3D GetSize(u8 mip = 0) const { return _size >> glm::u32vec3(mip); }

	 private:
		const GL::TextureSize3D _size;
	};

	class TextureCube final : public Texture
	{
	 public:
		TextureCube(gE::Window* window, const TextureSettings<TextureDimension::D1D>& settings, const TextureData& = {});
		NODISCARD ALWAYS_INLINE GL::TextureSize1D GetSize(u8 mip = 0) const { return _size >> mip; }

	 private:
		const GL::TextureSize1D _size;
	};
}

namespace PVR
{
	NODISCARD GL::Texture* Read(gE::Window* window, const char* path, GL::WrapMode = GL::WrapMode::Repeat, GL::FilterMode = GL::FilterMode::Linear);
	NODISCARD u8* Read(const char* path, PVR::Header& header);
}