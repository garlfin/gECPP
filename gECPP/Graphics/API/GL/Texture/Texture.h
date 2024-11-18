#pragma once

#include <GLAD/glad.h>
#include <Graphics/API/GL/GL.h>
#include <Graphics/Texture/Texture.h>

#define GE_ANISOTROPY_COUNT 8

// Type is more "integral" so I use a lowercase convention
// Opaque type to prevent accidental pointer-to-handle conversions
struct handle
{
	handle() = default;
	ALWAYS_INLINE explicit handle(u64 i) : ID(i) {};

	ALWAYS_INLINE handle& operator=(u64 o) { ID = o; return *this; }

	OPERATOR_CAST_CONST(u64, ID);

	u64 ID = 0;
};

namespace GL
{
	class FrameBuffer;

	CONSTEXPR_GLOBAL handle NullHandle = handle();

 	class Texture : public GLObject
	{
	 public:
		Texture(gE::Window* window, GLenum target, GPU::Texture& settings);
 		Texture() = default;

		inline void Bind() const override { glBindTexture(_target, ID); }

		ALWAYS_INLINE int32_t Use(int32_t slot) const { glBindTextureUnit(slot, ID); return slot; } // NOLINT

		inline u32 Bind(u32 unit, GLenum access, u8 mip = 0, GLenum format = 0) const
		{
			glBindImageTexture(unit, ID, mip, _target == GL_TEXTURE_3D, 0, access, format ? format : _settings->Format);
			return unit;
		}

		handle GetHandle();
		virtual void CopyFrom(const Texture&) = 0;

		explicit ALWAYS_INLINE operator handle() { return GetHandle(); }

 		GET(GPU::Texture&, Settings, *_settings);

 		GET_CONST(GLenum, Target, _target);
		GET_CONST(GLenum, Format, _settings->Format);
		GET_CONST(u8, MipCount, _settings->MipCount);

		~Texture() override;

	 private:
		handle _handle = NullHandle;
 		GPU::Texture* _settings = DEFAULT;
 		GLenum _target = DEFAULT;
	};

	class Texture2D final : protected GPU::Texture2D, public Texture
	{
		API_SERIALIZABLE(Texture2D, GPU::Texture2D);
		API_UNDERLYING_IMPL(GL::Texture);

	 public:
		NODISCARD ALWAYS_INLINE TextureSize2D GetSize(u8 mip = 0) const { return max(Size >> glm::u32vec2(mip), glm::u32vec2(1)); }

		void CopyFrom(const GL::Texture&) override;
	};

	class Texture3D final : protected GPU::Texture3D, public Texture
	{
		API_SERIALIZABLE(Texture3D, GPU::Texture3D);
		API_UNDERLYING_IMPL(GL::Texture);

	 public:
		NODISCARD ALWAYS_INLINE TextureSize3D GetSize(u8 mip = 0) const { return max(Size >> glm::u32vec3(mip), glm::u32vec3(1)); }

		void CopyFrom(const GL::Texture&) override;
	};

	class TextureCube final : protected GPU::TextureCube, public Texture
	{
		API_SERIALIZABLE(TextureCube, GPU::TextureCube);
		API_SETTINGS_OPERATOR(GPU::TextureCube);
		API_UNDERLYING_IMPL(GL::Texture);

	 public:
		NODISCARD ALWAYS_INLINE TextureSize1D GetSize(u8 mip = 0) const { return MAX(Size >> mip, 1); }

		void CopyFrom(const GL::Texture&) override;
	};
}

namespace PVR
{
	NODISCARD GL::Texture* Read(gE::Window*, const Path&, GPU::WrapMode = GPU::WrapMode::Repeat, GPU::FilterMode = GPU::FilterMode::Linear);
	NODISCARD Array<u8> Read(const Path& path, Header& header);
}