//
// Created by scion on 9/19/2023.
//

#include "VoxelPipeline.h"

#include <Window/Window.h>
#include <Entity/VoxelCapture.h>
#include <glm/gtx/string_cast.hpp>

#define MODE_TAA_COMBINE 0
#define MODE_TAA_VELOCITY 1
#define MODE_TAA_COPY 2
#define MODE_DOWNSAMPLE 3

#define VOXEL_TAA_GROUP_SIZE 4

#define VOXEL_SNAP 10.0

namespace gE::VoxelPipeline
{
	Buffers::Buffers(Window* window) :
		_voxelBuffer(window, 1, nullptr, GPU::BufferUsageHint::Dynamic)
	{
		_voxelBuffer.Bind(API::BufferTarget::ShaderStorage, 4);
	}

	Target3D::Target3D(VoxelCapture& capture, Camera3D& camera, ProbeSettings probeSettings) :
		RenderTarget(capture, camera),
		_probeSettings(probeSettings)
	{
		GetFrameBuffer().SetDefaultSize(camera.GetSize());
		Target3D::Resize();
	}

	void Target3D::Resize()
	{
		if(_color.GetSize() == GetCamera().GetSize()) return;

		PlacementNew(_color, &GetCamera().GetWindow(), GPU::Texture3D(ColorFormat, GetCamera().GetSize()));
		PlacementNew(_colorBack, &GetCamera().GetWindow(), GPU::Texture3D(ColorBackFormat, GetCamera().GetSize()));
	}

	void Target3D::RenderPass(float d, Camera* callingCamera)
	{
		Window& window = GetWindow();

		window.GetLights().UseNearestLights(glm::vec3(0.0f));

		window.RenderState = RenderState::Voxel;

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);
		GetCamera().SetViewport();

		_colorBack.Bind(0, GL_READ_WRITE, 0);

		window.GetRenderers().OnRender(d, &GetCamera());
	}

	bool Target3D::Setup(float d, Camera* camera)
	{
		if(!camera) return false;

		Buffers& buffers = GetWindow().GetVoxelBuffers();
		API::ComputeShader& voxelShader = GetWindow().GetVoxelComputeShader();
		Transform& transform = GetOwner().GetTransform();
		const TransformData& cameraTransform = camera->GetOwner().GetTransform().GetGlobalTransform();

		float cellSize = GetScale() / GetSize().x * VOXEL_SNAP;

		const glm::vec3 pos = floor(cameraTransform.Position / cellSize) * cellSize;
		const glm::ivec3 velocity = ceil(pos - transform->Position);

		transform.SetPosition(pos);
		transform.OnUpdate(0.f); // Force update on model matrix since it passed its tick.

		GetOwner().GetGPUVoxelScene(buffers.Scene);
		buffers.UpdateScene();

		glm::u16vec3 dispatchSize = DIV_CEIL_T(_colorBack.GetSize(), VOXEL_TAA_GROUP_SIZE, glm::u16vec3);

		voxelShader.Bind();

		_colorBack.Bind(0, GL_READ_WRITE);
		_color.Bind(2, GL_READ_WRITE);
		voxelShader.SetUniform(0, glm::ivec4(MODE_TAA_COPY));

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		voxelShader.Dispatch(dispatchSize);

		_color.Bind(0, GL_READ_WRITE);
		_colorBack.Bind(2, GL_READ_WRITE);
		voxelShader.SetUniform(0, glm::ivec4(velocity, MODE_TAA_VELOCITY));

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		voxelShader.Dispatch(dispatchSize);

		return true;
	}

	void Target3D::PostProcessPass(float d)
	{
		API::ComputeShader& voxelShader = GetWindow().GetVoxelComputeShader();

		_color.Bind(0, GL_READ_WRITE);
		_colorBack.Bind(2, GL_READ_WRITE);

		voxelShader.SetUniform(0, glm::ivec4(MODE_TAA_COMBINE));
		voxelShader.Dispatch(DIV_CEIL_T(_colorBack.GetSize(), VOXEL_TAA_GROUP_SIZE, glm::u16vec3));

		voxelShader.SetUniform(0, glm::ivec4(MODE_DOWNSAMPLE));
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

