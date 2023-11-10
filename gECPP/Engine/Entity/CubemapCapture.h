//
// Created by scion on 10/23/2023.
//

#pragma once
#include "Entity.h"
#include <Engine/Component/Camera.h>

namespace gE
{
	class CubemapCapture final : public Entity
	{
	 public:
		CubemapCapture(Window*, u16 size);

		GET_REGULAR(GL::TextureCube*, Texture, _camera.GetAttachment(0));
		GET(CameraCubemap&, Camera, _camera);

	 private:
		CameraCubemap _camera;
	};
}
