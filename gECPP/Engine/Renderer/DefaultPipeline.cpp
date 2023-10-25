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
		window->SetRenderState(RenderState::PreZ);

		glDepthMask(1);
		glColorMask(1, 1, 1, 1);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glColorMask(0, 0, 0, 0);

		window->GetRenderers().OnRender(0.f);

		// COLOR
		window->SetRenderState(RenderState::Color);

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);
		window->GetRenderers().OnRender(0.f);
	}

	void DefaultPipeline::RenderPass3D(Window*, Camera3D*) {}
	void DefaultPipeline::RenderPassDirectionalShadow(Window*, Camera2D*) {}

	#ifdef DEBUG
	// this will only really be used in; not too concerned w/ perf
	bool PostProcessPass::CheckRequirements(const Camera& cam) const
	{
		const AttachmentSettings& settings = cam.GetSettings().RenderAttachments;

		if(Requirements.Depth != settings.Depth) return false;
		if(Requirements.DepthCopy && !settings.DepthCopy) return false;

		for(u8 i = 0; i < GE_MAX_ATTACHMENTS; i++)
		{
			if(!Requirements.Attachments[i]) continue;
			if(Requirements.Attachments[i] != settings.Attachments[i]) return false;
			if(Requirements.ColorCopy[i] && !settings.ColorCopy[i]) return false;
		}
		return true;
	}
	#endif
}