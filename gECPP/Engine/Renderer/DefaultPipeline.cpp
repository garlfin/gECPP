//
// Created by scion on 9/29/2023.
//

#include "DefaultPipeline.h"
#include <Engine/Window.h>

#include <utility>

namespace gE
{
	gE::DefaultPipeline::Buffers::Buffers(Window* window) :
		_sceneBuffer(window), _cameraBuffer(window), _lightBuffer(window)
	{
		_sceneBuffer.Bind(GL::BufferTarget::Uniform, 0);
		_cameraBuffer.Bind(GL::BufferTarget::Uniform, 1);
		_lightBuffer.Bind(GL::BufferTarget::Uniform, 2);
	}

	DefaultPipeline::Target2D::Target2D(Camera2D& camera, std::vector<PostProcessEffect<Target2D>*> effects) :
		RenderTarget<Camera2D>(camera), IDepthTarget(_depth.Get()),
		_depth(GetFrameBuffer(), GL::TextureSettings2D(DefaultPipeline::DepthFormat, camera.GetSize())),
		_color(GetFrameBuffer(), GL::TextureSettings2D(DefaultPipeline::ColorFormat, camera.GetSize())),
		_velocity(GetFrameBuffer(), GL::TextureSettings2D(DefaultPipeline::VelocityFormat, camera.GetSize())),
		_colorBack(&GetWindow(), GL::TextureSettings2D(DefaultPipeline::ColorFormat, camera.GetSize())),
		_postProcessBack(&GetWindow(), GL::TextureSettings2D(DefaultPipeline::ColorFormat, camera.GetSize())),
		_effects(std::move(effects))
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

		window.GetRenderers().OnRender(0.f);

		// COLOR
		window.State = State::Color;

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);

		window.GetRenderers().OnRender(0.f);
		window.GetCubemaps().DrawSkybox();
	}

	void DefaultPipeline::Target2D::PostProcessPass(float)
	{
		GL::ComputeShader& taaShader = GetWindow().GetTAAShader();

		taaShader.Bind();

		_postProcessBack.Bind(0, GL_WRITE_ONLY);
		taaShader.SetUniform(0, _color->Use(0));
		taaShader.SetUniform(1, _colorBack.Use(1));
		taaShader.SetUniform(2, _velocity->Use(2));

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		taaShader.Dispatch(DIV_CEIL(_color->GetSize(), TAA_GROUP_SIZE));

		// Copy TAA result to taa "backbuffer"
		_colorBack.CopyFrom(_postProcessBack);

		// Post process loop
		GL::Texture2D* front = *_color, *back = &_postProcessBack;
		for(PostProcessEffect<Target2D>* effect : _effects)
		{
			std::swap(front, back);
			effect->RenderPass(*front, *back);
		}

		glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		// Sync post process "backbuffer" and main color buffer
		if(front == *_color) front->CopyFrom(*back);
	}

	void DefaultPipeline::Target2D::RenderDependencies(float delta)
	{
		GetWindow().GetLights().Sun->GetCamera().Draw(delta, &GetCamera());
	}
}