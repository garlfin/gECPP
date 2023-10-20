//
// Created by scion on 9/30/2023.
//

#include "VoxelCapture.h"
#include <Engine/Window.h>

gE::VoxelCapture::VoxelCapture(gE::Window* w, u16 resolution, float size, gE::Entity* p) : Entity(w, p),
	_camera(this, CameraSettings3D(
		SizelessCameraSettings((RenderPass) VoxelPipeline::RenderPass3D, { 0.01, resolution }, VoxelPipeline::Target3D),
		GL::TextureSize3D(resolution)
	)),
	_size(size),
	_resolution(resolution)
{
}

void gE::VoxelCapture::Update()
{
	VoxelPipeline::Buffers* buffers = GetWindow()->GetVoxelBuffers();
	Transform& transform = GetTransform();

	buffers->Scene.VoxelScale = _size / _resolution;
	buffers->Scene.Minimum = transform.Position - transform.Scale / 2.f;
	buffers->Scene.Maximum = transform.Position + transform.Scale / 2.f;
	buffers->Scene.Texture = GetColor()->Handle();

	buffers->UpdateScene();
}