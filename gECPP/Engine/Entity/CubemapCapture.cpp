//
// Created by scion on 10/23/2023.
//

#include "CubemapCapture.h"
#include <Engine/Window.h>

namespace gE
{
	SizelessCameraSettings CubemapSettings
		{
			(RenderPass) gE::DefaultPipeline::RenderPassCubemap,
			ClipPlanes(0.1, 100),
			CameraTiming(),
			gE::DefaultPipeline::AttachmentsDefault
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
		buffers.UpdateLighting(sizeof(GL::TextureHandle), offsetof(GL::Lighting, Skybox));

		Manager::OnRender(delta);
	}

	void CubemapManager::DrawSkybox()
	{
		return;
		if(!_skyboxMesh)
		{
			gETF::File skyboxCube;
			gETF::Read("Resource/Model/skybox.gETF", skyboxCube);

			_skyboxShader = CreateSmartPointer<GL::Shader>(_window, "Resource/Shader/skybox.vert", "Resource/Shader/skybox.frag");
			_skyboxMesh = skyboxCube.Meshes[0];
			_skyboxMesh->CreateVAO(_window);
		}

		_skyboxShader->Bind();

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_GEQUAL);

		_skyboxMesh->VAO->Draw(0);
	}
}
