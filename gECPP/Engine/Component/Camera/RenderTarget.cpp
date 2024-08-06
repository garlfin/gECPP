//
// Created by scion on 12/1/2023.
//

#include "RenderTarget.h"
#include "Camera.h"

namespace gE
{
	IRenderTarget::IRenderTarget(Entity& owner, Camera& camera) : API::Asset(&camera.GetWindow()),
		_camera(camera),
		_owner(owner),
		_frameBuffer(&camera.GetWindow())
	{}
}