//
// Created by scion on 9/30/2023.
//

#include "VoxelCapture.h"
#include <Engine/Window.h>

namespace gE
{
	CameraSettings3D CreateVoxelSettings(u16);

	VoxelCapture::VoxelCapture(gE::Window* w, u16 resolution, float size) : Entity(w),
		_camera(this, _target, resolution, size),
		_target(*this, _camera)
	{
	}

	void VoxelCamera::GetGLVoxelScene(GL::VoxelScene& scene)
	{
		Transform& transform = GetOwner()->GetTransform();

		scene.VoxelScale = _size / GetSize().x;
		scene.Minimum = transform.Position - transform.Scale / 2.f;
		scene.Maximum = transform.Position + transform.Scale / 2.f;
		scene.Texture = (handle) ((IColorTarget&) GetTarget()).GetColor();
	}

	CameraSettings3D CreateVoxelSettings(u16 resolution, float size)
	{
		return
		{
			ICameraSettings{ ClipPlanes(0.f, size), DefaultCameraTiming },
			glm::ivec3(resolution)
		};
	}

	VoxelCamera::VoxelCamera(Entity* o, Camera3D::TARGET_TYPE& t, u16 r, float s) :
		Camera3D(o, nullptr, t, CreateVoxelSettings(r)),
		_size(s)
	{
	}
}