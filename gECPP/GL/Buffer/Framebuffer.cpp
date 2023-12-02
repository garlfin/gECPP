//
// Created by scion on 9/20/2023.
//
#include "FrameBuffer.h"

void GL::FrameBuffer::SetAttachment(u8 i, GL::Texture& t)
{
	GE_ASSERT(i < GL_MAX_ATTACHMENTS, "INDEX OUT OF RANGE!");

	t.Attach(*this, GL_COLOR_ATTACHMENT0 + i, 0);
	_attachmentsEnum[i] = GL_COLOR_ATTACHMENT0 + i;
	glNamedFramebufferDrawBuffers(ID, GL_MAX_ATTACHMENTS, _attachmentsEnum);
}

void GL::FrameBuffer::SetNoAttatchments(const GL::TextureSize2D& size)
{
	glNamedFramebufferParameteri(ID, GL_FRAMEBUFFER_DEFAULT_WIDTH, size.x);
	glNamedFramebufferParameteri(ID, GL_FRAMEBUFFER_DEFAULT_HEIGHT, size.y);
	glNamedFramebufferParameteri(ID, GL_FRAMEBUFFER_DEFAULT_LAYERS, 1);
	glNamedFramebufferParameteri(ID, GL_FRAMEBUFFER_DEFAULT_SAMPLES, 1);

	memset((void*) _attachmentsEnum, GL_NONE, sizeof(_attachmentsEnum));
	glNamedFramebufferDrawBuffers(ID, GL_MAX_ATTACHMENTS, _attachmentsEnum);
}


