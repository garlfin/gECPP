//
// Created by scion on 11/28/2023.
//

#pragma once

#include <GL/Texture/Texture.h>
#include <Prototype.h>
#include "CameraAttachments.h"

namespace gE
{
	class PostProcessEffect
	{
	 public:
		explicit PostProcessEffect(Window* w, AttachmentSettings&);

		virtual void RenderPass(Camera&, GL::Texture&, GL::Texture&) = 0;
		GET_CONST(const AttachmentSettings&, Requirements, _requirements);

	 private:
		const AttachmentSettings _requirements;
		Window* const _window;
	};
}