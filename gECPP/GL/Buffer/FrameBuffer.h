#pragma once

#include <GL/GL.h>
#include <GL/Texture/Texture.h>

namespace GL
{
	enum class FramebufferTarget
	{
		Depth = GL_DEPTH_ATTACHMENT,
		DepthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
		Color = GL_COLOR_ATTACHMENT0
	};

	class FrameBuffer : public Asset
	{
	 public:
		inline FrameBuffer(gE::Window* win) : Asset(win) { glCreateFramebuffers(1, &ID); }

		inline void Attach(Texture* tex, FramebufferTarget t, uint8_t mip = 0)
		{
			glNamedFramebufferTexture(ID, (GLenum) t, tex->Get(), mip);
		}
		inline void Bind() const override { glBindFramebuffer(GL_FRAMEBUFFER, ID); }

		~FrameBuffer() override
		{
			glDeleteFramebuffers(1, &ID);
		}
	};
}