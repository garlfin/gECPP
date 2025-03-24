//
// Created by scion on 9/29/2023.
//

#include "DefaultPipeline.h"

#include <utility>
#include <Window.h>
#include <Entity/Light/DirectionalLight.h>
#include <Entity/Light/Light.h>
#include <glm/gtx/string_cast.hpp>

#define HIZ_MODE_COPY 0
#define HIZ_MODE_DOWNSAMPLE 1

#define HIZ_GROUP_SIZE 8

namespace gE
{
	DefaultPipeline::Buffers::Buffers(Window* window) :
		_cameraBuffer(window, 1, nullptr, GPU::BufferUsageHint::Dynamic, true),
		_sceneBuffer(window, 1, nullptr, GPU::BufferUsageHint::Dynamic, true),
		_lightBuffer(window, 1, nullptr, GPU::BufferUsageHint::Dynamic, true)
	{
		_sceneBuffer.Bind(API::BufferBaseTarget::Uniform, 0);
		_cameraBuffer.Bind(API::BufferBaseTarget::Uniform, 1);
		_lightBuffer.Bind(API::BufferBaseTarget::Uniform, 2);
	}

	void DefaultPipeline::Target2D::RenderPass(float delta, Camera*)
	{
		Camera2D& camera = RenderTarget::GetCamera();
		Window& window = camera.GetWindow();

		// PRE-Z
		window.RenderState = RenderState::PreZ;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		RenderTarget::GetCamera().SetViewport();

		window.GetRenderers().OnRender(0.f, &camera);

		API::ComputeShader& hiZShader = window.GetHiZShader();

		hiZShader.Bind();

		hiZShader.SetUniform(0, glm::vec4(HIZ_MODE_COPY, 0.0, camera.GetClipPlanes()));
		hiZShader.SetUniform(1, *_depth, 0);
		_linearDepth.Bind(0, GL_WRITE_ONLY, 0);

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		hiZShader.Dispatch(DIV_CEIL(_depth->GetSize(), HIZ_GROUP_SIZE));

		hiZShader.SetUniform(1, _linearDepth, 0);

		for(u8 i = 1; i < _linearDepth.GetMipCount(); i++)
		{
			Size2D mipSize = _linearDepth.GetSize(i);

			hiZShader.SetUniform(0, glm::vec4(HIZ_MODE_DOWNSAMPLE, i - 1, 0, 0));
			_linearDepth.Bind(0, GL_WRITE_ONLY, i);

			glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			hiZShader.Dispatch(DIV_CEIL(mipSize, HIZ_GROUP_SIZE));
		}

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		window.GetLights().UseNearestLights(glm::vec3(0.0f));
		window.GetCubemaps().UseNearestCubemaps(glm::vec3(0.0f));

		// COLOR
		window.RenderState = RenderState::PreZForward;

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);

		window.GetRenderers().OnRender(0.f, &camera);
		window.GetCubemaps().DrawSkybox();
	}

	void DefaultPipeline::Target2D::PostProcessPass(float)
	{
		_previousColor.CopyFrom(*_color);

		if(!RenderTarget::GetCamera().GetTiming().GetFrame())
			_previousDepth.CopyFrom(_linearDepth);

		// Post process loop
		API::Texture2D* front = &*_color, *back = &_postProcessBack;
		for(POSTPROCESS_T* effect : _effects)
			if(effect->RenderPass(*front, *back))
				std::swap(front, back);

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Sync post process "backbuffer" and main color buffer
		if(front == &_postProcessBack) _color->CopyFrom(_postProcessBack);
		_previousDepth.CopyFrom(_linearDepth);
	}

	DefaultPipeline::Target2D::Target2D(Entity& owner, Camera2D& camera, const std::vector<POSTPROCESS_T*>& effects) :
		RenderTarget(owner, camera),
		DepthTarget(camera, *_depth),
		ColorTarget(camera, *_color),
		_effects(effects)
	{
		Target2D::Resize();
	}

	void DefaultPipeline::Target2D::Resize()
	{
		if(_depth->GetSize() == GetSize()) return;

		PlacementNew(_depth, GetFrameBuffer(), GPU::Texture2D(DepthFormat, GetSize()));
		PlacementNew(_color, GetFrameBuffer(), GPU::Texture2D(ColorFormat, GetSize()));
		PlacementNew(_velocity, GetFrameBuffer(), GPU::Texture2D(VelocityFormat, GetSize()));
		PlacementNew(_linearDepth, &GetWindow(), GPU::Texture2D(HiZFormat, GetSize()));
		PlacementNew(_previousColor, &GetWindow(), GPU::Texture2D(ColorFormat, GetSize()));
		PlacementNew(_postProcessBack, &GetWindow(), GPU::Texture2D(ColorFormat, GetSize()));
		PlacementNew(_previousDepth, &GetWindow(), GPU::Texture2D(PreviousDepthFormat, GetSize()));

		if(!GetCamera().GetTiming().GetIsFirst())
			for(POSTPROCESS_T* effect : _effects)
				effect->Resize();
	}

	void DefaultPipeline::Target2D::GetGPUCameraOverrides(GPU::Camera& camera) const
	{
		camera.ColorTexture = (handle) _previousColor;
		camera.DepthTexture = (handle) _linearDepth;
	}

	void DefaultPipeline::Target2D::RenderDependencies(float delta)
	{
		Camera3D* reflectionSystem = GetWindow().GetReflectionSystem();
		LightManager& lightManager = GetWindow().GetLights();

		if(reflectionSystem) reflectionSystem->OnRender(delta, &RenderTarget::GetCamera());
		lightManager.Sun->GetCamera().OnRender(delta, &RenderTarget::GetCamera());
	}
}