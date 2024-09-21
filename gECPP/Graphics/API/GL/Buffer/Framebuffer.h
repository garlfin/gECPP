#pragma once

#include <Engine/AssetManager.h>
#include <GLAD/glad.h>
#include <Graphics/API/GL/GL.h>
#include <Graphics/Texture/Texture.h>

#define GL_MAX_ATTACHMENTS 4

namespace GL
{
	enum class FrameBufferTarget : GLenum
	{
		Depth = GL_DEPTH_ATTACHMENT,
		DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
		Color = GL_COLOR_ATTACHMENT0
	};

	NODISCARD ALWAYS_INLINE FrameBufferTarget operator+(FrameBufferTarget a, GLenum b)
	{
		return (FrameBufferTarget) ((GLenum) a + b);
	}

	class FrameBuffer : public GLObject
	{
	 public:
		explicit FrameBuffer(gE::Window* win);

		void SetDefaultSize(TextureSize2D);
		inline void Bind() const override { glBindFramebuffer(GL_FRAMEBUFFER, ID); }

		void SetDepthAttachment(Texture& h);
		void SetAttachment(u8, Texture&);

		ALWAYS_INLINE static void Reset() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

		inline ~FrameBuffer() override { glDeleteFramebuffers(1, &ID); }

	 private:
		GLenum _attachmentsEnum[GL_MAX_ATTACHMENTS] {};
	};
}