//
// Created by scion on 9/30/2023.
//

#include "VoxelCapture.h"
#include <Engine/Window.h>

namespace gE
{
	CameraSettings3D CreateVoxelSettings(u16);

	VoxelCapture::VoxelCapture(gE::Window* w, u16 resolution, float size) : Entity(w),
		_camera(this, nullptr, _target, CreateVoxelSettings(resolution)),
		_target(_camera),
		_size(size)
	{
	}

	void VoxelCapture::OnUpdate(float)
	{
		VoxelPipeline::Buffers& buffers = GetWindow().GetVoxelBuffers();
		Transform& transform = GetTransform();

		buffers.Scene.VoxelScale = _size / _camera.GetSize().x;
		buffers.Scene.Minimum = transform.Position - transform.Scale / 2.f;
		buffers.Scene.Maximum = transform.Position + transform.Scale / 2.f;
		buffers.Scene.Texture = (handle) GetColor();
		buffers.UpdateScene();
	}

	CameraSettings3D CreateVoxelSettings(u16 resolution, float size)
	{
		return
		{
			ICameraSettings(ClipPlanes(0.f, size), DefaultCameraTiming),
			glm::ivec3(resolution)
		};
	}
}