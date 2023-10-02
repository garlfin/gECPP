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

		GET_CONST_VALUE(GLenum, Format, Format);
		GET_CONST_VALUE(GLenum, Target, Target);
		GET_CONST_VALUE(GLenum, MipCount, Mips);

		TextureHandle Handle();

		void Attach(GL::FrameBuffer* buffer, GLenum attachment, u8 mip) const override;

		~Texture() override;

	 protected:
		Texture(gE::Window* window, GLenum tgt, const SizelessTextureSettings& settings);

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
		NODISCARD ALWAYS_INLINE GL::TextureSize2D GetSize(u8 mip = 0) { return _size >> glm::u32vec2(mip); }

	 private:
		const GL::TextureSize2D _size;
	};

	class Texture3D final : public Texture
	{
	 public:
		Texture3D(gE::Window* window, const TextureSettings<TextureDimension::D3D>& settings, const TextureData& = {});
		NODISCARD ALWAYS_INLINE GL::TextureSize3D GetSize(u8 mip = 0) { return _size >> glm::u32vec3(mip); }

	 private:
		const GL::TextureSize3D _size;
	};

	class TextureCubemap final : public Texture
	{
	 public:
		TextureCubemap(gE::Window* window, const TextureSettings<TextureDimension::D1D>& settings, const TextureData& = {});
		NODISCARD ALWAYS_INLINE GL::TextureSize1D GetSize(u8 mip = 0) { return _size >> mip; }

	 private:
		const GL::TextureSize1D _size;
	};
}

namespace PVR
{
	GL::Texture2D* Read(gE::Window* window, const char* path, GL::WrapMode wrapMode, GL::FilterMode filterMode);
}