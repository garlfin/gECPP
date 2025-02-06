//
// Created by scion on 9/20/2023.
//
#include "Framebuffer.h"

namespace GL
{
	void Framebuffer::SetAttachment(u8 i, Texture& t)
	{
		GE_ASSERTM(i < GL_MAX_ATTACHMENTS, "INDEX OUT OF RANGE!");

		_attachmentsEnum[i] = GL_COLOR_ATTACHMENT0 + i;

		glNamedFramebufferTexture(ID, _attachmentsEnum[i], t.Get(), 0);
		glNamedFramebufferDrawBuffers(ID, GL_MAX_ATTACHMENTS, _attachmentsEnum);
	}

	void Framebuffer::SetDefaultSize(TextureSize2D size)
	{
		glNamedFramebufferParameteri(ID, GL_FRAMEBUFFER_DEFAULT_WIDTH, size.x);
		glNamedFramebufferParameteri(ID, GL_FRAMEBUFFER_DEFAULT_HEIGHT, size.y);
		glNamedFramebufferParameteri(ID, GL_FRAMEBUFFER_DEFAULT_LAYERS, 1);
		glNamedFramebufferParameteri(ID, GL_FRAMEBUFFER_DEFAULT_SAMPLES, 1);

		memset(_attachmentsEnum, GL_NONE, sizeof(_attachmentsEnum));
		glNamedFramebufferDrawBuffers(ID, GL_MAX_ATTACHMENTS, _attachmentsEnum);
	}

	Framebuffer::Framebuffer(gE::Window* win) : APIObject(win)
	{
		glCreateFramebuffers(1, &ID);
	}

	void Framebuffer::SetDepthAttachment(Texture& h)
	{
		glNamedFramebufferTexture(ID, GL_DEPTH_ATTACHMENT, h.Get(), 0);
	}
}
