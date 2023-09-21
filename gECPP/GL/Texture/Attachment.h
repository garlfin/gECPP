//
// Created by scion on 9/20/2023.
//

#pragma once

namespace GL
{
	class FrameBuffer;

	class Attachment
	{
	 public:
		virtual void Attach(GL::FrameBuffer* buffer, GLenum attachment, u8 mip) const = 0;
	};
}
