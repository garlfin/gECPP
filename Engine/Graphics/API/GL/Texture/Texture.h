#pragma once

#include <GLAD/glad.h>
#include <Graphics/API/GL/GL.h>
#include <Graphics/Texture/Texture.h>

#define GE_ANISOTROPY_COUNT 8

using handle = u64;

namespace GL
{
	CONSTEXPR_GLOBAL handle NullHandle = 0;

 	class Texture : public GLObject, public Underlying
	{
 		DEFAULT_OPERATOR_MOVE(Texture);
 		DELETE_OPERATOR_COPY(Texture);

	public:
		Texture(gE::Window* window, GLenum target, GPU::Texture& settings);
 		Texture() = default;

 		ALWAYS_INLINE i32 Use(i32 slot) const { glBindTextureUnit(slot, ID); return slot; } // NOLINT
		inline void Bind() const override { glBindTexture(_target, ID); }
		u32 Bind(u32 unit, GLenum access, u8 mip = 0, GLenum format = 0) const
		{
			glBindImageTexture(unit, ID, mip, _target == GL_TEXTURE_3D, 0, access, format ? format : _settings->Format);
			return unit;
		}

 		explicit ALWAYS_INLINE operator handle() const { return GetHandle(); }
		NODISCARD handle GetHandle() const;

		virtual void CopyFrom(const Texture&) = 0;

 		GET(GPU::Texture&, Settings, *_settings);
 		GET_CONST(GLenum, Target, _target);
 		GET_CONST(GLenum, Format, _settings->Format);
		GET_CONST(u8, MipCount, _settings->MipCount);

		~Texture() override;

 	protected:
 		void UpdateParameters() const;

	private:
		mutable handle _handle = NullHandle;
 		GPU::Texture* _settings = DEFAULT;
 		GLenum _target = DEFAULT;
	};

	class Texture1D final : protected GPU::Texture1D, public Texture
	{
		API_SERIALIZABLE(Texture1D, GPU::Texture1D);
		API_DEFAULT_CM_CONSTRUCTOR(Texture1D);
		API_UNDERLYING_IMPL();

	public:
		NODISCARD ALWAYS_INLINE Size1D GetSize(u8 mip = 0) const { return std::max<Size1D>(Size >> mip, 1); }

		void CopyFrom(const GL::Texture&) override;
		void UpdateParameters() override { GL::Texture::UpdateParameters(); }
	};

	class Texture2D final : protected GPU::Texture2D, public Texture
	{
		API_SERIALIZABLE(Texture2D, GPU::Texture2D);
		API_DEFAULT_CM_CONSTRUCTOR(Texture2D);
		API_UNDERLYING_IMPL();
		REFLECTABLE_ONGUI_PROTO(GPU::Texture2D);
		REFLECTABLE_ICON_PROTO();

	public:
		NODISCARD ALWAYS_INLINE Size2D GetSize(u8 mip = 0) const { return max(Size >> glm::u32vec2(mip), glm::u32vec2(1)); }

		void CopyFrom(const GL::Texture&) override;
		void UpdateParameters() override { GL::Texture::UpdateParameters(); }
	};

	class Texture3D final : protected GPU::Texture3D, public Texture
	{
		API_SERIALIZABLE(Texture3D, GPU::Texture3D);
		API_DEFAULT_CM_CONSTRUCTOR(Texture3D);
		API_UNDERLYING_IMPL();

	public:
		NODISCARD ALWAYS_INLINE Size3D GetSize(u8 mip = 0) const { return max(Size >> glm::u32vec3(mip), glm::u32vec3(1)); }

		void CopyFrom(const GL::Texture&) override;
		void UpdateParameters() override { GL::Texture::UpdateParameters(); }
	};

	class TextureCube final : protected GPU::TextureCube, public Texture
	{
		API_SERIALIZABLE(TextureCube, GPU::TextureCube);
		API_DEFAULT_CM_CONSTRUCTOR(TextureCube);
		API_UNDERLYING_IMPL();

	public:
		NODISCARD ALWAYS_INLINE Size1D GetSize(u8 mip = 0) const { return MAX(Size >> mip, 1); }

		void CopyFrom(const GL::Texture&) override;
		void UpdateParameters() override { GL::Texture::UpdateParameters(); }
	};
}