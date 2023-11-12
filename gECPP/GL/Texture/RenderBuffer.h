//
// Created by scion on 9/20/2023.
//

#pragma once

#include <GL/gl.h>
#include <GLAD/glad.h>
#include "Attachment.h"
#include "TextureSettings.h"

namespace GL
{
	class RenderBuffer : public Asset, public Attachment
	{
	 public:
		RenderBuffer(gE::Window* window, const TextureSettings<TextureDimension::D2D>& settings)
			: Asset(window), _size(settings.Size)
		{
			glCreateRenderbuffers(1, &ID);
			glNamedRenderbufferStorage(ID, settings.Format, _size.x, _size.y);
		}

		GET_CONST(const TextureSize2D&, Size, _size);

		void Bind() const override { glBindRenderbuffer(GL_RENDERBUFFER, ID); }
		void Attach(GL::FrameBuffer* buffer, GLenum attachment, u8 mip) const override;

		~RenderBuffer() override { glDeleteRenderbuffers(1, &ID); }

	 private:
		const TextureSize2D _size;
	};
}