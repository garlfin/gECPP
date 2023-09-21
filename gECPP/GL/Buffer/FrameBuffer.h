#pragma once

#include <GL/GL.h>
#include <GL/Texture/Texture.h>
#include <Engine/AssetManager.h>

#define FRAMEBUFFER_MAX_COLOR_ATTACHMENTS 2

namespace GL
{
	enum class FrameBufferTarget : GLenum
	{
		Depth = GL_DEPTH_ATTACHMENT,
		DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
		Color = GL_COLOR_ATTACHMENT0
	};

	NODISCARD ALWAYS_INLINE FrameBufferTarget operator+(FrameBufferTarget a, GLenum b) { return FrameBufferTarget((GLenum) a + b); }

	class FrameBuffer : public Asset
	{
	 public:
		inline explicit FrameBuffer(gE::Window* win) : Asset(win) { glCreateFramebuffers(1, &ID); }

		inline void Bind() const override { glBindFramebuffer(GL_FRAMEBUFFER, ID); }

		ALWAYS_INLINE void SetDepthAttachment(Attachment* h) { h->Attach(this, GL_DEPTH_ATTACHMENT, 0); }
		void SetAttachment(u8 i, Attachment* h);
		void SetNoAttatchments(const GL::TextureSize2D& size);

		inline ~FrameBuffer() override { glDeleteFramebuffers(1, &ID); }

	 private:
		GLenum _attachmentsEnum[FRAMEBUFFER_MAX_COLOR_ATTACHMENTS] {};
	};
}