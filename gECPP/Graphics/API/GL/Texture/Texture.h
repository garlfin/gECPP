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

		inline void Bind() const override { glBindTexture(_target, ID); }

		ALWAYS_INLINE int32_t Use(int32_t slot) const { glBindTextureUnit(slot, ID); return slot; } // NOLINT

		inline u32 Bind(u32 unit, GLenum access, u8 mip = 0, GLenum format = 0) const
		{
			glBindImageTexture(unit, ID, mip, _target == GL_TEXTURE_3D, 0, access, format ?: _settings.Format);
			return unit;
		}

 		virtual void CopyFrom(const Texture&) = 0;

 		GET_CONST(const GPU::ITextureSettings&, Settings, _settings);
 		GET_CONST(u32, MipCount, _settings.MipCount);
 		GET_CONST(GLenum, Target, _target);

		handle GetHandle();
		explicit ALWAYS_INLINE operator handle() const { return _handle; }

		~Texture() override;

	 private:
 		const GPU::ITextureSettings& _settings;
		GLenum _target;
		handle _handle = NullHandle;
	};

	class Texture2D final : public GPU::Texture2D, public Texture
	{
		SERIALIZABLE_PROTO_T_BODY(Texture2D, GPU::Texture2D);
	 public:
		Texture2D(gE::Window* window, const GPU::TextureSettings2D& settings, GPU::TextureData&& d = {}) :
			GPU::Texture2D(settings, std::move(d)), GL::Texture(window, GL_TEXTURE_2D, Settings) { Construct(); }

		void CopyFrom(const GL::Texture&) override;

	 private:
		void Construct();
	};

	class Texture3D final : public GPU::Texture3D, public Texture
	{
		SERIALIZABLE_PROTO_T_BODY(Texture3D, GPU::Texture3D);
	 public:
		Texture3D(gE::Window* window, const GPU::TextureSettings3D& settings, GPU::TextureData&& d = {}) :
			GPU::Texture3D(settings, std::move(d)), GL::Texture(window, GL_TEXTURE_3D, Settings) { Construct(); }

		void CopyFrom(const GL::Texture&) override;

	 private:
		void Construct();
	};

	class TextureCube final : public GPU::TextureCube, public Texture
	{
		SERIALIZABLE_PROTO_T_BODY(TextureCube, GPU::TextureCube);
	 public:
		TextureCube(gE::Window* window, const GPU::TextureSettings1D& settings, GPU::TextureData&& d = {}) :
			GPU::TextureCube(settings, std::move(d)), GL::Texture(window, GL_TEXTURE_CUBE_MAP, Settings) { Construct(); }

		void CopyFrom(const GL::Texture&) override;

	 private:
		void Construct();
	};
}

#include "Texture.inl"