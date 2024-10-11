//
// Created by scion on 9/30/2023.
//

#include "VoxelCapture.h"
#include <Engine/Window.h>

namespace gE
{
	CameraSettings3D CreateVoxelSettings(u16, float);

	VoxelCapture::VoxelCapture(Window* w, u16 resolution, float size) : Entity(w, Flags(true, UINT8_MAX)),
		_camera(this, _target, CreateVoxelSettings(resolution, size)),
		_target(*this, _camera)
	{
		GetTransform().SetScale() = glm::vec3(size);
	}

	void VoxelCapture::GetGPUVoxelScene(API::VoxelScene& scene)
	{
		Transform& transform = GetTransform();

		scene.Center = transform->Position;
		scene.Scale = transform->Scale.x;
		scene.Color = (handle) GetTarget().GetColor();
	}

	CameraSettings3D CreateVoxelSettings(u16 resolution, float size)
	{
		return
		{
			ICameraSettings{ ClipPlanes(0.f, size), DefaultCameraTiming },
			glm::ivec3(resolution)
		};
	}
}