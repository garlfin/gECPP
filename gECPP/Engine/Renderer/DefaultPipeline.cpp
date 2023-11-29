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

	void DefaultPipeline::RenderPass2D(Camera2D& camera)
	{
		const glm::u32vec2& cameraSize = camera.GetSize();
		Window& window = camera.GetWindow();

		// PRE-Z
		window.Stage = RenderStage::PreZ;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, cameraSize.x, cameraSize.y);

		window.GetRenderers().OnRender(0.f);

		// COLOR
		window.Stage = RenderStage::Color;

		glDepthMask(0);
		glColorMask(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		window.GetRenderers().OnRender(0.f);
		window.GetCubemaps().DrawSkybox();
	}

	void DefaultPipeline::RenderPass3D(Camera3D& camera) {}

	void DefaultPipeline::RenderPassShadow(Camera2D& camera)
	{
		const glm::u32vec2& cameraSize = camera.GetSize();
		Window& window = camera.GetWindow();

		window.Stage = RenderStage::PreZ;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, cameraSize.x, cameraSize.y);

		window.GetRenderers().OnRender(0.f);
	}

	void DefaultPipeline::RenderPassCubemap(CameraCubemap& camera)
	{
		const u32 cameraSize = camera.GetSize();
		Window& window = camera.GetWindow();

		window.Stage = RenderStage::PreZ;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, cameraSize, cameraSize);

		window.GetRenderers().OnRender(0.f);
	}
}