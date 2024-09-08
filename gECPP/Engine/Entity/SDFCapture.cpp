//
// Created by scion on 7/14/2024.
//

#include "VoxelCapture.h"
#include <Engine/Window.h>

namespace gE
{
	CameraSettings3D CreateSDFSettings(u16);

	SDFCapture::SDFCapture(Window* w, u16 resolution) :
		Entity(w, Flags(true, UINT8_MAX)),
		_camera(this, _target, CreateSDFSettings(resolution)),
		_target(*this, _camera)
	{

	}

	void SDFCapture::GetGLSDFScene(API::SDFScene& scene)
	{

	}

	CameraSettings3D CreateSDFSettings(u16 size)
	{
		return CameraSettings3D
		{
			ICameraSettings { ClipPlanes(0.01, 100.0), DefaultCameraTiming },
			glm::u16vec3(size)
		};
	}
}