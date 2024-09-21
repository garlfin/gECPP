//
// Created by scion on 9/19/2023.
//

#include "VoxelPipeline.h"

#include <Engine/Window.h>
#include <Engine/Entity/VoxelCapture.h>

#define MODE_TAA_COMBINE 0
#define MODE_TAA_DOWNSAMPLE 1
#define MODE_TAA_VELOCITY 2
#define MODE_TAA_COPY 3

namespace gE::VoxelPipeline
{
	Buffers::Buffers(Window* window) : _voxelBuffer(window, 1)
	{
		_voxelBuffer.Bind(API::BufferTarget::Uniform, 4);
	}

	Target3D::Target3D(VoxelCapture& capture, Camera3D& camera) :
		RenderTarget(capture, camera),
		_color(&camera.GetWindow(), { ColorFormat, camera.GetSize()}),
		_colorBack(&camera.GetWindow(), { ColorBackFormat, camera.GetSize() })
	{
		GetFrameBuffer().SetDefaultSize(camera.GetSize());
	}

	void Target3D::RenderPass(float d, Camera* camera)
	{
		Window& window = GetWindow();
		TextureSize2D size = GetSize();

		window.State = State::Voxel;

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);
		glViewport(0, 0, size.x, size.y);

		_colorBack.Bind(0, GL_READ_WRITE, 0);

		window.GetRenderers().OnRender(d, &GetCamera());
	}

	bool Target3D::Setup(float d, Camera* camera)
	{
		if(!camera) return false;

		Buffers& buffers = GetWindow().GetVoxelBuffers();
		API::ComputeShader& voxelShader = GetWindow().GetVoxelTAAShader();
		Transform& transform = GetOwner().GetTransform();
		Transform& cameraTransform = camera->GetOwner()->GetTransform();

		float cellSize = GetScale() * 2.f / GetSize().x;

		glm::ivec3 pos = floor(cameraTransform->Position / cellSize);
		_velocity = pos - glm::ivec3(transform->Position / cellSize);
		transform.SetPosition() = glm::vec3(pos) * cellSize;
		transform.OnUpdate(0.f); // Force update on model matrix since it passed its tick.

		GetOwner().GetGLVoxelScene(buffers.Scene);
		buffers.UpdateScene();

		glm::u16vec3 dispatchSize = DIV_CEIL_T(_colorBack.GetSize(), VOXEL_TAA_GROUP_SIZE, glm::u16vec3);

		voxelShader.Bind();

		_colorBack.Bind(0, GL_READ_WRITE);
		_color.Bind(2, GL_READ_WRITE);
		voxelShader.SetUniform(0, glm::ivec4(_velocity, MODE_TAA_COPY));

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		voxelShader.Dispatch(dispatchSize);

		_color.Bind(0, GL_READ_WRITE);
		_colorBack.Bind(2, GL_READ_WRITE);
		voxelShader.SetUniform(0, glm::ivec4(_velocity, MODE_TAA_VELOCITY));

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		voxelShader.Dispatch(dispatchSize);

		return true;
	}

	void Target3D::PostProcessPass(float d)
	{
		API::ComputeShader& voxelShader = GetWindow().GetVoxelTAAShader();

		voxelShader.Bind();

		_color.Bind(0, GL_READ_WRITE);
		_colorBack.Bind(2, GL_READ_WRITE);

		voxelShader.SetUniform(0, glm::ivec4(MODE_TAA_COMBINE));
		voxelShader.Dispatch(DIV_CEIL_T(_colorBack.GetSize(), VOXEL_TAA_GROUP_SIZE, glm::u16vec3));

		voxelShader.SetUniform(0, glm::ivec4(MODE_TAA_DOWNSAMPLE));
		for(u8 i = 1; i < _color.GetMipCount(); i++)
		{
			_color.Bind(0, GL_READ_WRITE, i);
			_color.Bind(2, GL_READ_WRITE, i - 1);

			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			voxelShader.Dispatch(DIV_CEIL_T(_colorBack.GetSize(i), VOXEL_TAA_GROUP_SIZE, glm::u16vec3));
		}

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}

