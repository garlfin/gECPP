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

	void DefaultPipeline::Target2D::RenderPass()
	{
		Camera2D& camera = GetCamera();
		glm::u32vec2 size = camera.GetSize();
		Window& window = camera.GetWindow();

		// PRE-Z
		window.Stage = RenderStage::PreZ;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, size.x, size.y);

		window.GetRenderers().OnRender(0.f);

		// COLOR
		window.Stage = RenderStage::Color;

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);

		window.GetRenderers().OnRender(0.f);
		window.GetCubemaps().DrawSkybox();
	}

	void DefaultPipeline::Target2D::PostProcessPass()
	{
		Camera2D& camera = GetCamera();
		glm::u32vec2 size = camera.GetSize();
		GL::ComputeShader& taaShader = GetWindow().GetTAAShader();

		GL::Texture2D* front = (GL::Texture2D*) _color, *back = &_postProcessBack;

		_velocity->Bind(0, GL_READ_ONLY);
		_postProcessBack.Bind(1, GL_WRITE_ONLY);

		taaShader.SetUniform(0, _color->Use(0));
		taaShader.SetUniform(1, _colorBack.Use(1));

		taaShader.Bind();
		taaShader.Dispatch(DIV_CEIL(_color->GetSize(), TAA_GROUP_SIZE));

		// Copy TAA result to taa "backbuffer"
		_colorBack.CopyFrom(*back);

		// Post process loop here
		for(PostProcessEffect<Target2D>* effect : _effects)
		{
			std::swap(front, back);
			effect->RenderPass(*this, *front, *back);
		}

		// Sync post process "backbuffer" and main color buffer
		front->CopyFrom(*back);
	}
}