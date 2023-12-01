//
// Created by scion on 12/1/2023.
//

#include "RenderTarget.h"
#include "Camera.h"

namespace gE
{
	IRenderTarget::IRenderTarget(Camera& camera) : GL::Asset(&camera.GetWindow()),
		_camera(camera),
		_frameBuffer(&camera.GetWindow())
	{
	}
}