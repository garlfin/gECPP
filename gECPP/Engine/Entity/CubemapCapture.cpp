//
// Created by scion on 10/23/2023.
//

#include "CubemapCapture.h"
#include <Engine/Window.h>

namespace gE
{
	CONSTEXPR_GLOBAL ICameraSettings CubemapCameraSettings
	{
		ClipPlanes(0.1, 100),
		DefaultCameraTiming
	};

	CubemapCapture::CubemapCapture(gE::Window* w, u16 size) : Entity(w),
		_camera(this, nullptr, _target, { CubemapCameraSettings, size }),
		_target(_camera)
	{
	}

	void CubemapCapture::GetGLCubemap(GL::CubemapData& cubemap)
	{
	}

	void CubemapManager::OnRender(float delta)
	{
		DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();

		buffers.Lighting.Skybox = Skybox->GetHandle();
		buffers.UpdateLighting(sizeof(handle), offsetof(GL::Lighting, Skybox));

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

	void CubemapTarget::RenderPass(float, Camera*)
	{
		CameraCubemap& camera = GetCamera();
		Window& window = camera.GetWindow();
		u32 size = GetCamera().GetSize();

		window.State = State::Cubemap;

		glDepthMask(1);
		glColorMask(1, 1, 1, 1);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, size, size);

		window.GetRenderers().OnRender(0.f);
	}

	CONSTEXPR_GLOBAL GL::ITextureSettings CubemapColorSettings
	{
		GL_RGB16F,
		GL::WrapMode::Clamp,
		GL::FilterMode::Linear
	};

	CubemapTarget::CubemapTarget(CameraCubemap& camera) : RenderTarget<CameraCubemap>(camera),
		_color(&camera.GetWindow(), { CubemapColorSettings, camera.GetSize() })
	{
	}
}
