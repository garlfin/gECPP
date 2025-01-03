//
// Created by scion on 9/19/2023.
//

#include "VoxelPipeline.h"

#include <Engine/Window/Window.h>
#include <Engine/Entity/VoxelCapture.h>
#include <glm/gtx/string_cast.hpp>

#define MODE_TAA_COMBINE 0
#define MODE_TAA_VELOCITY 1
#define MODE_TAA_COPY 2
#define MODE_DOWNSAMPLE 3
#define MODE_CALCULATE_PROBES 4
#define MODE_CALCULATE_GI 5

#define VOXEL_TAA_GROUP_SIZE 4

namespace gE::VoxelPipeline
{
	GPU::Texture1D CreateProbeSettings(ProbeSettings);

	Buffers::Buffers(Window* window) :
		_voxelBuffer(window, 1, nullptr, GPU::BufferUsageHint::Dynamic)
	{
		_voxelBuffer.Bind(API::BufferTarget::ShaderStorage, 4);
	}

	Target3D::Target3D(VoxelCapture& capture, Camera3D& camera, ProbeSettings probeSettings) :
		RenderTarget(capture, camera),
		_color(&camera.GetWindow(), { ColorFormat, camera.GetSize() }),
		_colorBack(&camera.GetWindow(), { ColorBackFormat, camera.GetSize() }),
		_probeSettings(probeSettings)
	{
		GetFrameBuffer().SetDefaultSize(camera.GetSize());
	}

	void Target3D::RenderPass(float d, Camera* camera)
	{
		Window& window = GetWindow();
		TextureSize2D size = GetSize();

		window.GetLights().UseNearestLights(glm::vec3(0.0f));

		window.RenderState = RenderState::Voxel;

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
		API::ComputeShader& voxelShader = GetWindow().GetVoxelComputeShader();
		Transform& transform = GetOwner().GetTransform();
		const TransformData& cameraTransform = camera->GetOwner().GetTransform().GetGlobalTransform();

		float cellSize = GetScale() / GetSize().x * 10.f;

		glm::vec3 pos = floor(cameraTransform.Position / cellSize) * cellSize;
		glm::vec3 oldPos = transform->Position;

		glm::ivec3 velocity = (pos - oldPos) / cellSize;

		transform.SetPosition(pos);
		transform.OnUpdate(0.f); // Force update on model matrix since it passed its tick.

		GetOwner().GetGPUVoxelScene(buffers.Scene);
		buffers.UpdateScene(offsetof(API::VoxelScene, Probes));

		glm::u16vec3 dispatchSize = DIV_CEIL_T(_colorBack.GetSize(), VOXEL_TAA_GROUP_SIZE, glm::u16vec3);

		voxelShader.Bind();

		_colorBack.Bind(0, GL_READ_WRITE);
		_color.Bind(2, GL_READ_WRITE);
		voxelShader.SetUniform(0, glm::ivec4(velocity, MODE_TAA_COPY));

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

		voxelShader.Bind();

		_color.Bind(0, GL_READ_WRITE);
		_colorBack.Bind(2, GL_READ_WRITE);

		voxelShader.SetUniform(0, glm::ivec4(MODE_CALCULATE_PROBES));
		voxelShader.Dispatch(DIV_CEIL_T(_probeSettings.GridResolution, VOXEL_TAA_GROUP_SIZE, glm::u16vec3));

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

		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		voxelShader.SetUniform(0, glm::ivec4(MODE_CALCULATE_PROBES));
		voxelShader.Dispatch(DIV_CEIL_T(_probeSettings.GridResolution, VOXEL_TAA_GROUP_SIZE, glm::u16vec3));

		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
	}

	GPU::Texture1D CreateProbeSettings(ProbeSettings s)
	{
		const u32 probeCount = s.GridResolution.x * s.GridResolution.y * s.GridResolution.z;

		return
		{
			ProbeFormat,
			probeCount * GE_VOXEL_SAMPLE_DIRECTIONS
		};
	}
}

