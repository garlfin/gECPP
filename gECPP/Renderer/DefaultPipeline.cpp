//
// Created by scion on 9/29/2023.
//

#include "DefaultPipeline.h"

#include <Window/Window.h>
#include <Entity/Light/DirectionalLight.h>
#include <Entity/Light/Light.h>

#include <utility>
#include <glm/gtx/string_cast.hpp>

#define HIZ_MODE_COPY 0
#define HIZ_MODE_DOWNSAMPLE 1

#define HIZ_GROUP_SIZE 8

namespace gE
{
	DefaultPipeline::Buffers::Buffers(Window* window) :
		_cameraBuffer(window, 1, nullptr, GPU::BufferUsageHint::Dynamic),
		_sceneBuffer(window, 1, nullptr, GPU::BufferUsageHint::Dynamic),
		_lightBuffer(window, 1, nullptr, GPU::BufferUsageHint::Dynamic)
	{
		_sceneBuffer.Bind(API::BufferTarget::Uniform, 0);
		_cameraBuffer.Bind(API::BufferTarget::Uniform, 1);
		_lightBuffer.Bind(API::BufferTarget::Uniform, 2);
	}

	void DefaultPipeline::Target2D::RenderPass(float delta, Camera*)
	{
		Camera2D& camera = GetCamera();
		glm::u32vec2 size = camera.GetSize();
		Window& window = camera.GetWindow();

		// PRE-Z
		window.RenderState = RenderState::PreZ;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, size.x, size.y);

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
			TextureSize2D mipSize = _linearDepth.GetSize(i);

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

		if(!GetCamera().GetTiming().GetFrame())
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
		RenderTarget(owner, camera), IDepthTarget(*_depth), IColorTarget(*_color),
		_depth(GetFrameBuffer(), GPU::Texture2D(DepthFormat, camera.GetSize())),
		_color(GetFrameBuffer(), GPU::Texture2D(ColorFormat, camera.GetSize())),
		_velocity(GetFrameBuffer(), GPU::Texture2D(VelocityFormat, camera.GetSize())),
		_linearDepth(&GetWindow(), GPU::Texture2D(HiZFormat, camera.GetSize())),
		_previousColor(&GetWindow(), GPU::Texture2D(ColorFormat, camera.GetSize())),
		_postProcessBack(&GetWindow(), GPU::Texture2D(ColorFormat, camera.GetSize())),
		_previousDepth(&GetWindow(), GPU::Texture2D(PreviousDepthFormat, camera.GetSize())),
		_effects(effects)
	{
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

		if(reflectionSystem) reflectionSystem->OnRender(delta, &GetCamera());
		lightManager.Sun->GetCamera().OnRender(delta, &GetCamera());
	}
}