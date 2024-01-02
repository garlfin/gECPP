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
		_colorBack(&camera.GetWindow(), { VoxelPipeline::ColorBackFormat, camera.GetSize()}),
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

		_color.Bind(0, GL_READ_WRITE, 0);
		_data.Bind(1, GL_READ_WRITE, 0);

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

	void Target3D::PostProcessPass(float d)
	{
		GL::ComputeShader& voxelShader = GetWindow().GetVoxelTAAShader();

		voxelShader.Bind();

		_colorBack.Bind(0, GL_READ_WRITE);
		_color.Bind(2, GL_READ_ONLY);

		// voxelShader.SetUniform(0, 0u);
		// voxelShader.Dispatch(DIV_CEIL_T(GetSize(), VOXEL_TAA_GROUP_SIZE, glm::u16vec3));
		// glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		//_color.CopyFrom(_colorBack);

		//glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		voxelShader.SetUniform(0, 1u);
		voxelShader.Dispatch(DIV_CEIL_T(GetSize(), VOXEL_TAA_GROUP_SIZE, glm::u16vec3));
		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}

