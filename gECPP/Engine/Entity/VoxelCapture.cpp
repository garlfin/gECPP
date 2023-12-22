//
// Created by scion on 9/30/2023.
//

#include "VoxelCapture.h"
#include <Engine/Window.h>

namespace gE
{
	CameraSettings3D CreateVoxelSettings(u16);

	VoxelCapture::VoxelCapture(gE::Window* w, u16 r, float s) : Entity(w),
		_camera(this, _target, CreateVoxelSettings(r)),
		_target(*this, _camera),
		_size(s)
	{
	}

	void VoxelCapture::GetGLVoxelScene(GL::VoxelScene& scene)
	{
		Transform& transform = GetTransform();

		scene.VoxelScale = _size / _camera.GetSize().x;
		scene.Minimum = transform.Position - transform.Scale / 2.f;
		scene.Maximum = transform.Position + transform.Scale / 2.f;

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