#pragma once

#include <Core/AssetManager.h>
#include <GLAD/glad.h>
#include <Graphics/API/GL/GL.h>
#include <Graphics/Texture/Texture.h>

#define GL_MAX_ATTACHMENTS 4

namespace GL
{
	enum class FramebufferTarget : GLenum
	{
		Depth = GL_DEPTH_ATTACHMENT,
		DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
		Color = GL_COLOR_ATTACHMENT0
	};

	NODISCARD ALWAYS_INLINE FramebufferTarget operator+(FramebufferTarget a, GLenum b)
	{
		return (FramebufferTarget) ((GLenum) a + b);
	}

	class Framebuffer : public GLObject
	{
	 public:
		explicit Framebuffer(gE::Window* win);

		void SetDefaultSize(Size2D);
		inline void Bind() const override { glBindFramebuffer(GL_FRAMEBUFFER, ID); }

		void SetDepthAttachment(Texture& h);
		void SetAttachment(u8, Texture&);

		ALWAYS_INLINE static void Reset() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

		inline ~Framebuffer() override { glDeleteFramebuffers(1, &ID); }

	 private:
		GLenum _attachmentsEnum[GL_MAX_ATTACHMENTS] {};
	};
}