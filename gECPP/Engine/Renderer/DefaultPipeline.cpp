//
// Created by scion on 9/29/2023.
//

#include "DefaultPipeline.h"

#include <Engine/Window.h>
#include <Engine/Entity/Light/DirectionalLight.h>
#include <Engine/Entity/Light/Light.h>

#include <utility>

#define HIZ_MODE_COPY 0
#define HIZ_MODE_DOWNSAMPLE 1

#define TAA_GROUP_SIZE 8
#define HIZ_GROUP_SIZE 8

namespace gE
{
	DefaultPipeline::Buffers::Buffers(Window* window) :
		_cameraBuffer(window, 1), _sceneBuffer(window, 1), _lightBuffer(window, 1)
	{
		_sceneBuffer.Bind(API::BufferTarget::Uniform, 0);
		_cameraBuffer.Bind(API::BufferTarget::Uniform, 1);
		_lightBuffer.Bind(API::BufferTarget::Uniform, 2);
	}

	DefaultPipeline::Target2D::Target2D(Entity& owner, Camera2D& camera, const std::vector<PostProcessEffect<Target2D>*>& effects) :
		RenderTarget(owner, camera), IDepthTarget(*_depth), IColorTarget(*_color),
		_depth(GetFrameBuffer(), GPU::TextureSettings2D(DepthFormat, camera.GetSize())),
		_color(GetFrameBuffer(), GPU::TextureSettings2D(ColorFormat, camera.GetSize())),
		_velocity(GetFrameBuffer(), GPU::TextureSettings2D(VelocityFormat, camera.GetSize())),
		_taaBack(&GetWindow(), GPU::TextureSettings2D(ColorFormat, camera.GetSize())),
		_depthBack(&GetWindow(), GPU::TextureSettings2D(HiZFormat, camera.GetSize())),
		_postProcessBack(&GetWindow(), GPU::TextureSettings2D(ColorFormat, camera.GetSize())),
		_effects(effects)
	{
	}

	void DefaultPipeline::Target2D::RenderPass(float delta, Camera*)
	{
		Camera2D& camera = GetCamera();
		glm::u32vec2 size = camera.GetSize();
		Window& window = camera.GetWindow();

		// PRE-Z
		window.State = State::PreZ;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, size.x, size.y);

		window.GetRenderers().OnRender(0.f, &camera);

		API::ComputeShader& hiZShader = window.GetHiZShader();

		hiZShader.Bind();

		hiZShader.SetUniform(0, glm::vec4(HIZ_MODE_COPY, 0.0, camera.GetClipPlanes()));
		hiZShader.SetUniform(1, *_depth, 0);
		_depthBack.Bind(0, GL_WRITE_ONLY, 0);

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		hiZShader.Dispatch(DIV_CEIL(_depth->GetSize(), HIZ_GROUP_SIZE));

		hiZShader.SetUniform(1, _depthBack, 0);

		for(u8 i = 1; i < _depthBack.GetMipCount(); i++)
		{
			TextureSize2D mipSize = _depthBack.GetSize(i);

			hiZShader.SetUniform(0, glm::vec4(HIZ_MODE_DOWNSAMPLE, i - 1, 0, 0));
			_depthBack.Bind(0, GL_WRITE_ONLY, i);

			glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
			hiZShader.Dispatch(DIV_CEIL(mipSize, HIZ_GROUP_SIZE));
		}

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		Buffers& buf = GetWindow().GetPipelineBuffers();

		buf.Camera.ColorTexture = (handle) _taaBack;
		buf.Camera.DepthTexture = (handle) _depthBack;
		buf.UpdateCamera(sizeof(handle) * 2, offsetof(GPU::Camera, ColorTexture));

		window.GetLights().UseNearestLights(glm::vec3(0.0f));
		window.GetCubemaps().UseNearestCubemaps(glm::vec3(0.0f));

		// COLOR
		window.State = State::Color;

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);

		window.GetRenderers().OnRender(0.f, &camera);
		window.GetCubemaps().DrawSkybox();
	}

	void DefaultPipeline::Target2D::PostProcessPass(float)
	{
		API::ComputeShader& taaShader = GetWindow().GetTAAShader();

		taaShader.Bind();

		_postProcessBack.Bind(0, GL_WRITE_ONLY);
		taaShader.SetUniform(0, _color->Use(0));
		taaShader.SetUniform(1, _taaBack.Use(1));
		taaShader.SetUniform(2, _velocity->Use(2));

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		taaShader.Dispatch(DIV_CEIL(_color->GetSize(), TAA_GROUP_SIZE));

		// Copy TAA result to taa "backbuffer"
		_taaBack.CopyFrom(_postProcessBack);

		// Post process loop
		API::Texture2D* front = &*_color, *back = &_postProcessBack;
		for(PostProcessEffect<Target2D>* effect : _effects)
		{
			std::swap(front, back);
			effect->RenderPass(*front, *back);
		}

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		// Sync post process "backbuffer" and main color buffer
		if(front == &*_color) front->CopyFrom(*back);
	}

	void DefaultPipeline::Target2D::RenderDependencies(float delta)
	{
		Camera3D* reflectionSystem = GetWindow().GetReflectionSystem();
		LightManager& lightManager = GetWindow().GetLights();

		if(reflectionSystem) reflectionSystem->OnRender(delta, &GetCamera());
		lightManager.Sun->GetCamera().OnRender(delta, &GetCamera());
	}
}