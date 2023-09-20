//
// Created by scion on 9/20/2023.
//
#include "FrameBuffer.h"

void GL::FrameBuffer::SetAttachment(FramebufferTarget target, gE::Handle<Texture>& h)
{
	if(target == FramebufferTarget::Depth) return SetDepthAttachment(h);
	SetAttachment(GLenum(target) - GL_COLOR_ATTACHMENT0, h);
}

void GL::FrameBuffer::SetNoAttatchments(const GL::TextureSize2D& size)
{
	glNamedFramebufferParameteri(ID, GL_FRAMEBUFFER_DEFAULT_WIDTH, size.x);
	glNamedFramebufferParameteri(ID, GL_FRAMEBUFFER_DEFAULT_HEIGHT, size.y);
}
