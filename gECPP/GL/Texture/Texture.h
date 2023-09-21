#pragma once

#include <GL/gl.h>
#include <gEModel/gETF/Prototype.h>
#include <GL/Binary.h>
#include <GLAD/glad.h>
#include "TextureSettings.h"
#include "Attachment.h"

namespace GL
{
	typedef u64 TextureHandle;

	class Texture : public Asset, public Attachment
	{
	 public:
		ALWAYS_INLINE uint32_t Use(uint32_t slot) const { glBindTextureUnit(slot, ID); return slot; }
		ALWAYS_INLINE u32 Bind(u32 unit, GLenum access, u8 mip = 0) const { glBindImageTexture(unit, ID, mip, GL_FALSE, 0, access, Format); return unit; }
		inline void Bind() const override { glBindTexture(Target, ID); }

		GET_CONST(GLenum, Format, Format);
		GET_CONST(GLenum, Target, Target);
		GET_CONST(GLenum, MipCount, Mips);

		void Attach(GL::FrameBuffer* buffer, GLenum attachment, u8 mip) const override;

		~Texture() override { glDeleteTextures(1, &ID); }

	 protected:
		Texture(gE::Window* window, GLenum tgt, const SizelessTextureSettings& settings);

		const uint8_t Mips;
		const GLenum Format;
		const GLenum Target;
	};

	class Texture2D final : public Texture
	{
	 public:
		Texture2D(gE::Window* window, const TextureSettings<TextureDimension::D2D>& settings, const TextureData& = {});
		NODISCARD ALWAYS_INLINE GL::TextureSize2D GetSize(u8 mip = 0) { return Size >> glm::u32vec2(mip); }

	 protected:
		const GL::TextureSize2D Size;
	};

	class Texture3D final : public Texture
	{
	 public:
		Texture3D(gE::Window* window, const TextureSettings<TextureDimension::D3D>& settings, const TextureData& = {});
		NODISCARD ALWAYS_INLINE GL::TextureSize3D GetSize(u8 mip = 0) { return Size >> glm::u32vec3(mip); }

	 protected:
		const GL::TextureSize3D Size;
	};
}

namespace PVR
{
	GL::Texture2D* Read(gE::Window* window, const char* path, GL::WrapMode wrapMode, GL::FilterMode filterMode);
}