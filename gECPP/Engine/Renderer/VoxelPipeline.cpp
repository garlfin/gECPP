//
// Created by scion on 9/19/2023.
//

#include "VoxelPipeline.h"
#include <Engine/Entity/VoxelCapture.h>
#include <Engine/Window.h>

namespace gE::VoxelPipeline
{
	Buffers::Buffers(gE::Window* window) : _voxelBuffer(window)
	{
		_voxelBuffer.Bind(GL::BufferTarget::Uniform, 4);
	}

	Target3D::Target3D(VoxelCapture& capture, Camera3D& camera) : RenderTarget<Camera3D>(capture, camera),
		_color(&camera.GetWindow(), { VoxelPipeline::ColorFormat, camera.GetSize() }),
		_data(&camera.GetWindow(), { VoxelPipeline::DataFormat, camera.GetSize() })
	{
		GetFrameBuffer().SetDefaultSize(camera.GetSize());
	}

	void Target3D::RenderPass(float d, Camera* camera)
	{
		Window& window = GetWindow();
		GL::TextureSize2D size = GetSize();

		window.State = gE::State::Voxel;

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);
		glViewport(0, 0, size.x, size.y);

		GetColor().Bind(0, GL_READ_WRITE, 0);
		GetData().Bind(1, GL_READ_WRITE, 0);

		window.GetRenderers().OnRender(d);
	}

	bool Target3D::Setup(float d, Camera* camera)
	{
		if(!camera) return false;

		VoxelPipeline::Buffers& buffers = GetWindow().GetVoxelBuffers();
		Transform& transform = GetOwner().GetTransform();
		Transform& cameraTransform = camera->GetOwner()->GetTransform();

		// transform.Position = glm::floor(cameraTransform.Position);
		// transform.OnRender(0.f);

		GetOwner().GetGLVoxelScene(buffers.Scene);
		buffers.UpdateScene();

		return true;
	}
}

