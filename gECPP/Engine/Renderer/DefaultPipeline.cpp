//
// Created by scion on 9/29/2023.
//

#include "DefaultPipeline.h"
#include <Engine/Window.h>

namespace gE
{
	gE::DefaultPipeline::Buffers::Buffers(Window* window)
		: _sceneBuffer(window), _cameraBuffer(window)
	{
		_sceneBuffer.Bind(GL::BufferTarget::Uniform, 0);
		_cameraBuffer.Bind(GL::BufferTarget::Uniform, 1);
	}

	void DefaultPipeline::RenderPass2D(Window* window, Camera2D* camera)
	{
		// PRE-Z
		window->SetRenderStage(RenderStage::PreZ);

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);

		window->GetRenderers().OnRender(0.f);

		// COLOR
		window->SetRenderStage(RenderStage::Color);

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);

		window->GetRenderers().OnRender(0.f);
	}

	void DefaultPipeline::RenderPass3D(Window*, Camera3D*) { }

	void DefaultPipeline::RenderPassDirectionalShadow(Window*, Camera2D*) { }

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