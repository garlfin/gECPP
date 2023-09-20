#pragma once

#include <GL/GL.h>
#include <GL/Texture/Texture.h>
#include <Engine/AssetManager.h>

#define FRAMEBUFFER_MAX_COLOR_ATTACHMENTS 2

namespace GL
{
	enum class FramebufferTarget : GLenum
	{
		Depth = GL_DEPTH_ATTACHMENT,
		DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
		Color = GL_COLOR_ATTACHMENT0
	};

	NODISCARD ALWAYS_INLINE FramebufferTarget operator+(FramebufferTarget a, GLenum b) { return FramebufferTarget((GLenum) a + b); }

	class FrameBuffer : public Asset
	{
	 public:
		inline explicit FrameBuffer(gE::Window* win) : Asset(win) { glCreateFramebuffers(1, &ID); }

		ALWAYS_INLINE void Attach(Texture* tex, FramebufferTarget t, uint8_t mip = 0)
		{
			glNamedFramebufferTexture(ID, (GLenum) t, tex->Get(), mip);
		}

		inline void Bind() const override { glBindFramebuffer(GL_FRAMEBUFFER, ID); }

		GET(Texture*, Depth, _depthStencil.Get());
		NODISCARD ALWAYS_INLINE Texture* GetAttachment(u8 i) const { assert(i < FRAMEBUFFER_MAX_COLOR_ATTACHMENTS); return _attachments[i].Get(); }

		ALWAYS_INLINE void SetDepthAttachment(gE::Handle<Texture>& h)
		{
			SetAttachment(FramebufferTarget::Depth, h.Get(), 0);
			_depthStencil = h;
		}

		ALWAYS_INLINE void SetAttachment(u8 i, gE::Handle<Texture>& h)
		{
			assert(i < FRAMEBUFFER_MAX_COLOR_ATTACHMENTS);
			SetAttachment(FramebufferTarget::Color + i, h.Get(), 0);
			_attachments[i] = h;
		}

		void SetAttachment(FramebufferTarget target, gE::Handle<Texture>& h);
		void SetNoAttatchments(const GL::TextureSize2D& size)

		inline ~FrameBuffer() override { glDeleteFramebuffers(1, &ID); }

	 private:
		gE::Handle<Texture> _depthStencil;
		gE::Handle<Texture> _attachments[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS];

		ALWAYS_INLINE void SetAttachment(FramebufferTarget target, Texture* tex, u8 m) { glNamedFramebufferTexture(ID, (GLenum) target, tex->Get(), m); }
	};
}