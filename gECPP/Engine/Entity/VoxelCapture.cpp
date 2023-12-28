//
// Created by scion on 9/30/2023.
//

#include "VoxelCapture.h"
#include <Engine/Window.h>

namespace gE
{
	CameraSettings3D CreateVoxelSettings(u16, float);

	VoxelCapture::VoxelCapture(gE::Window* w, u16 r, float s) : Entity(w, Flags(true, UINT8_MAX)),
		_camera(this, _target, CreateVoxelSettings(r, s)),
		_target(*this, _camera)
	{
		GetTransform().Scale = glm::vec3(s);
	}

	void VoxelCapture::GetGLVoxelScene(GL::VoxelScene& scene)
	{
		Transform& transform = GetTransform();

		scene.Center = transform.Position;
		scene.Scale = transform.Scale.x;
		scene.CellCount = GetTarget().GetSize().x;
		scene.Color = (handle) GetTarget().GetColor();
		scene.Data = (handle) GetTarget().GetData();
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