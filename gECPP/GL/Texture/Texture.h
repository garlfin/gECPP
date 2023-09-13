#pragma once

#include <GL/gl.h>
#include <gEModel/gETF/Prototype.h>
#include <GL/Binary.h>
#include <GLAD/glad.h>
#include "TextureSettings.h"

namespace GL
{
	template<TextureDimension DIMENSION>
	class Texture : public Asset
	{
	 public:
		NODISCARD ALWAYS_INLINE TextureSize<DIMENSION> GetSize(u8 mip = 0) const { return Size >> TextureSize<DIMENSION>(mip); }
		ALWAYS_INLINE uint32_t Use(uint32_t slot) const { glBindTextureUnit(slot, ID); return slot; }
		ALWAYS_INLINE u32 Bind(u32 unit, GLenum access, u8 mip = 0) const { glBindImageTexture(unit, ID, mip, GL_FALSE, 0, access, Format); return unit; }
		inline void Bind() const override { glBindTexture(Target, ID); }

		GET_CONST(GLenum, Format, Format);
		GET_CONST(GLenum, Target, Target);
		GET_CONST(GLenum, MipCount, Mips);

		~Texture() override { glDeleteTextures(1, &ID); }

	 protected:
		Texture(gE::Window* window, GLenum tgt, const TextureSettings<DIMENSION>& settings);

		const TextureSize<DIMENSION> Size;
		const uint8_t Mips;
		const GLenum Format;
		const GLenum Target;
	};

	template class Texture<TextureDimension::D2D>;
	template class Texture<TextureDimension::D3D>;

	class Texture2D final : public Texture<TextureDimension::D2D>
	{
	 public:
		Texture2D(gE::Window* window, const TextureSettings<TextureDimension::D2D>& settings, const TextureData& = {});
	};

	class Texture3D final : public Texture<TextureDimension::D3D>
	{
	 public:
		Texture3D(gE::Window* window, const TextureSettings<TextureDimension::D3D>& settings, const TextureData& = {});
	};

	class TextureHandle
	{
	 public:
		TextureHandle(const Texture<TextureDimension::D2D>&); // NOLINT
		TextureHandle(const Texture<TextureDimension::D3D>&); // NOLINT

		ALWAYS_INLINE uint32_t Use(uint32_t slot) const { glBindTextureUnit(slot, _id); return slot; }
		ALWAYS_INLINE u32 Bind(u32 unit, GLenum access, u8 mip = 0) const { glBindImageTexture(unit, _id, mip, GL_FALSE, 0, access, _format); return unit; }
		ALWAYS_INLINE void Bind() const { glBindTexture(_target, _id); }

	 private:
		const GLuint _id;
		const GLuint _format;
		const GLuint _target;
	};
}

namespace PVR
{
	GL::Texture<GL::TextureDimension::D2D>* Read(gE::Window* window, const char* path, GL::WrapMode wM, GL::FilterMode fM);
}