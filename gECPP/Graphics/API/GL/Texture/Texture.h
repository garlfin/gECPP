#pragma once

#include <Graphics/API/GL/GL.h>
#include <Graphics/Texture/Texture.h>
#include <Graphics/Texture/TextureSettings.h>
#include <Engine/Binary/Binary.h>
#include <GLAD/glad.h>

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

 		virtual void CopyFrom(const Texture&) = 0;

 		GET_CONST(const GPU::ITextureSettings&, Settings, Settings);
 		GET_CONST(const GPU::TextureData&, Data, Data);

 		GET_CONST(u32, MipCount, Settings.MipCount);

		handle GetHandle();
		explicit ALWAYS_INLINE operator handle() const { return _handle; }

		~Texture() override;

	 protected:
 		const GPU::ITextureSettings& Settings;
 		const GPU::TextureData& Data;
		GLenum Target;

	 private:
		handle _handle = NullHandle;
	};

	class Texture2D final : public GPU::Texture2D, public Texture
	{
	 public:
		Texture2D(gE::Window* window, const GPU::TextureSettings2D& settings, GPU::TextureData&& = {});

		void CopyFrom(const GL::Texture&) override;
	};

	class Texture3D final : public GPU::Texture3D, public Texture
	{
	 public:
		Texture3D(gE::Window* window, const GPU::TextureSettings3D& settings, GPU::TextureData&& = {});

		void CopyFrom(const GL::Texture&) override;
	};

	class TextureCube final : public GPU::TextureCube, public Texture
	{
	 public:
		TextureCube(gE::Window* window, const GPU::TextureSettings1D& settings, GPU::TextureData&& = {});

		void CopyFrom(const GL::Texture&) override {};
	};
}