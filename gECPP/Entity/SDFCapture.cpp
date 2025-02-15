//
// Created by scion on 7/14/2024.
//

#include <Window/Window.h>

namespace gE
{
	CameraSettings3D CreateSDFSettings(u16);

	SDFCapture::SDFCapture(Window* window, u16 resolution) :
		Entity(window, nullptr, LayerMask::All, EntityFlags(true)),
		_camera(this, _target, CreateSDFSettings(resolution)),
		_target(*this, _camera)
	{

	}

	void SDFCapture::GetGPUSDFScene(GPU::SDFScene& scene)
	{

	}

	CameraSettings3D CreateSDFSettings(u16 size)
	{
		return CameraSettings3D
		{
			ICameraSettings { ClipPlanes(0.01, 100.0), DEFAULT },
			glm::u16vec3(size)
		};
	}
}