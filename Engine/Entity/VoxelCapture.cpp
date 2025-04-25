//
// Created by scion on 9/30/2023.
//

#include "VoxelCapture.h"

#include <Window.h>

namespace gE
{
	CameraSettings3D CreateVoxelSettings(u16, float);

	VoxelCapture::VoxelCapture(Window* w, VoxelCaptureSettings settings) :
		Entity(w, nullptr, LayerMask::All, EntityFlags(false)),
		_camera(this, _target, CreateVoxelSettings(settings.Resolution, settings.Size)),
		_target(*this, _camera, settings.ProbeSettings)
	{
		GetTransform().SetScale((vec3) (settings.Size * 0.5f));
	}

	void VoxelCapture::GetGPUVoxelScene(GPU::VoxelScene& scene)
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
			ICameraSettings{ ClipPlanes(0.f, size), DEFAULT },
			ivec3(resolution)
		};
	}
}
