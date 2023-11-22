//
// Created by scion on 9/29/2023.
//

#include "DefaultPipeline.h"
#include <Engine/Window.h>

namespace gE
{
	gE::DefaultPipeline::Buffers::Buffers(Window* window)
		: _sceneBuffer(window), _cameraBuffer(window), _lightBuffer(window)
	{
		_sceneBuffer.Bind(GL::BufferTarget::Uniform, 0);
		_cameraBuffer.Bind(GL::BufferTarget::Uniform, 1);
		_lightBuffer.Bind(GL::BufferTarget::Uniform, 2);
	}

	void DefaultPipeline::RenderPass2D(Window* window, Camera2D* camera)
	{
		const glm::u32vec2& cameraSize = camera->GetSize();

		// PRE-Z
		window->Stage = RenderStage::PreZ;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, cameraSize.x, cameraSize.y);

		window->GetRenderers().OnRender(0.f);

		// COLOR
		window->Stage = RenderStage::Color;

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		window->GetRenderers().OnRender(0.f);
		window->GetCubemaps().DrawSkybox();
	}

	void DefaultPipeline::RenderPass3D(Window* window, Camera3D* camera) {}

	void DefaultPipeline::RenderPassShadow(Window* window, Camera2D* camera)
	{
		const glm::u32vec2& cameraSize = camera->GetSize();

		window->Stage = RenderStage::PreZ;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, cameraSize.x, cameraSize.y);

		window->GetRenderers().OnRender(0.f);
	}

	void DefaultPipeline::RenderPassCubemap(Window*, CameraCubemap*)
	{

	}

#ifdef DEBUG
	// this will only really be used in debug; not too concerned w/ perf
	bool PostProcessPass::CheckRequirements(const Camera& cam) const
	{
		const AttachmentSettings& settings = cam.GetSettings().RenderAttachments;

		if(settings.Depth && Requirements.Depth != settings.Depth) return false;
		if(Requirements.CopyDepth && !settings.CopyDepth) return false;

		for(u8 i = 0; i < GE_MAX_ATTACHMENTS; i++)
		{
			if(!Requirements.Attachments[i]) continue;
			if(Requirements.Attachments[i] != settings.Attachments[i]) return false;
			if(Requirements.CopyAttachment[i] && !settings.CopyAttachment[i]) return false;
		}
		return true;
	}
#endif
}