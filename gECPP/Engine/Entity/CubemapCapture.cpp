//
// Created by scion on 10/23/2023.
//

#include "CubemapCapture.h"
#include <Engine/Window.h>

namespace gE
{
	ICameraSettings CubemapSettings
		{
			(RenderPass) gE::DefaultPipeline::RenderPassCubemap,
			ClipPlanes(0.1, 100),
			CameraTiming(),
			gE::DefaultPipeline::DepthFormat,
			gE::DefaultPipeline::ColorFormat
		};

	CubemapCapture::CubemapCapture(gE::Window* w, u16 size) :
		Entity(w), _camera(this, nullptr, { CubemapSettings, size })
	{
	}

	void CubemapCapture::GetGLCubemap(GL::CubemapData& cubemap)
	{
	}

	void CubemapManager::OnRender(float delta)
	{
		DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();

		buffers.Lighting.Skybox = Skybox->GetHandle();
		buffers.UpdateLighting(sizeof(GL::handle), offsetof(GL::Lighting, Skybox));

		Manager::OnRender(delta);
	}

	void CubemapManager::DrawSkybox()
	{
		if(!_skyboxVAO)
		{
			gETF::File skybox;
			gETF::Read("Resource/Model/skybox.gETF", skybox);

			GL::VAOSettings skyboxSettings;
			skybox.Meshes[0]->GetVAOSettings(skyboxSettings);

			_skyboxShader = CreateSmartPointer<GL::Shader>(_window, "Resource/Shader/skybox.vert", "Resource/Shader/skybox.frag");
			_skyboxVAO = CreateSmartPointer<GL::VAO>(_window, skyboxSettings);
		}

		_skyboxShader->Bind();

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);

		_skyboxVAO->Draw(0);
	}
}
