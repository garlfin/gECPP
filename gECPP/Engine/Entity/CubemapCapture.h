//
// Created by scion on 10/23/2023.
//

#pragma once
#include "Entity.h"
#include <Engine/Component/Camera.h>

namespace gE
{
	class CubemapCapture final : public CameraCubemap
	{
	 public:
		CubemapCapture(Window*, u16 size);

		GET_CONST(GL::TextureCube*, Texture, GetAttachment(0));
	};
}
