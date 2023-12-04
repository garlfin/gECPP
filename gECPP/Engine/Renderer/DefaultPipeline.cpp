//
// Created by scion on 9/29/2023.
//

#include "DefaultPipeline.h"
#include <Engine/Window.h>

namespace gE
{
	gE::DefaultPipeline::Buffers::Buffers(Window* window) :
		_sceneBuffer(window), _cameraBuffer(window), _lightBuffer(window)
	{
		_sceneBuffer.Bind(GL::BufferTarget::Uniform, 0);
		_cameraBuffer.Bind(GL::BufferTarget::Uniform, 1);
		_lightBuffer.Bind(GL::BufferTarget::Uniform, 2);
	}

	DefaultPipeline::Target2D::Target2D(Camera2D& camera) : RenderTarget<Camera2D>(camera), IDepthTarget(_depth.Get()),
		_depth(GetFrameBuffer(), GL::TextureSettings2D(DefaultPipeline::DepthFormat, camera.GetSize())),
		_color(GetFrameBuffer(), GL::TextureSettings2D(DefaultPipeline::ColorFormat, camera.GetSize())),
		_velocity(GetFrameBuffer(), GL::TextureSettings2D(DefaultPipeline::VelocityFormat, camera.GetSize())),
		_colorBack(&GetWindow(), GL::TextureSettings2D(DefaultPipeline::ColorFormat, camera.GetSize())),
		_postProcessBack(&GetWindow(), GL::TextureSettings2D(DefaultPipeline::ColorFormat, camera.GetSize()))
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
		glClear(GL_COLOR_BUFFER_BIT);

		window.GetRenderers().OnRender(0.f);
		window.GetCubemaps().DrawSkybox();
	}

	void DefaultPipeline::Target2D::PostProcessPass()
	{
		Camera2D& camera = GetCamera();
		glm::u32vec2 size = camera.GetSize();
		Window& window = camera.GetWindow();
		GL::ComputeShader& taaShader = GetWindow().GetTAAShader();

		GL::Texture2D* front = (GL::Texture2D*) _color, *back = &_postProcessBack;

		_color->Bind(0, GL_READ_ONLY);
		_velocity->Bind(1, GL_READ_ONLY);
		_postProcessBack.Bind(2, GL_WRITE_ONLY);
		taaShader.SetUniform(0, _colorBack.Use(0));

		taaShader.Bind();
		taaShader.Dispatch(DIV_CEIL(_color->GetSize(), TAA_GROUP_SIZE));

		// Copy TAA result to taa "backbuffer"
		_colorBack.CopyFrom(*back);

		// Post process loop here
		// for(PostProcessEffect effect* : PostProcessEffects)...

		// Sync post process "backbuffer" and main color buffer
		front->CopyFrom(*back);
	}
}