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

	CubemapCapture::CubemapCapture(gE::Window* w, u16 size) :
		Entity(w, Flags(true, UINT8_MAX)),
		Managed<CubemapCapture>(*this, GetWindow().GetCubemaps()),
		_camera(this, _target, { CubemapCameraSettings, size }),
		_target(_camera)
	{}

	void CubemapCapture::GetGLCubemap(GL::Cubemap& cubemap)
	{
		Transform& transform = GetTransform();

		cubemap.Position = transform.GetGlobalTransform().Position;
		cubemap.Scale = transform.GetGlobalTransform().Scale;
		cubemap.BlendRadius = 0.f;
		cubemap.Type = GL::CubemapType::AABB;
		cubemap.Color = (handle) GetColor();
	}

	void CubemapManager::OnRender(float delta)
	{
		DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();
		GL::Lighting& lighting = buffers.Lighting;

		lighting.Skybox = Skybox->GetHandle();
		lighting.CubemapCount = 1;
		((CubemapCapture*) at(0))->GetGLCubemap(lighting.Cubemaps[0]);

		buffers.UpdateLighting(sizeof(handle), offsetof(GL::Lighting, Skybox));
		buffers.UpdateLighting(sizeof(GL::Cubemap), offsetof(GL::Lighting, Cubemaps[0]));

		for(CubemapCapture* c : *this) c->GetCamera().OnRender(delta);
	}

	void CubemapManager::DrawSkybox()
	{
		if(!_skyboxVAO)
		{
			gETF::File skybox;
			gETF::Read("Resource/Model/skybox.gETF", skybox);

			GL::VAOSettings skyboxSettings;
			skybox.Meshes[0]->GetVAOSettings(skyboxSettings);

			_skyboxShader = ptr_create<GL::Shader>(_window, "Resource/Shader/skybox.vert", "Resource/Shader/skybox.frag");
			_skyboxVAO = ptr_create<GL::VAO>(_window, skyboxSettings);
		}

		_skyboxShader->Bind();

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);

		_skyboxVAO->Draw(0, _window->State.InstanceMultiplier);
	}

	void CubemapTarget::RenderPass(float, Camera*)
	{
		CameraCubemap& camera = GetCamera();
		Window& window = camera.GetWindow();

		window.State = State::Cubemap;

		glDepthMask(1);
		glColorMask(1, 1, 1, 1);
		glViewport(0, 0, GetSize(), GetSize());
		glClear(GL_DEPTH_BUFFER_BIT);

		window.GetRenderers().OnRender(0.f);
		window.GetCubemaps().DrawSkybox();
	}

	CONSTEXPR_GLOBAL GL::ITextureSettings CubemapColorFormat
	{
		GL_R11F_G11F_B10F,
		GL::WrapMode::Clamp,
		GL::FilterMode::Linear
	};

	CubemapTarget::CubemapTarget(CameraCubemap& camera) :
		RenderTarget<CameraCubemap>(*camera.GetOwner(), camera), IDepthTarget(_depth.Get()),
		_color(GetFrameBuffer(), GL::TextureSettings1D{ CubemapColorFormat, camera.GetSize() }),
		_depth(GetFrameBuffer(), GL::TextureSettings1D{ DefaultPipeline::DepthFormat, camera.GetSize() })
	{
	}

	void CubemapTarget::RenderDependencies(float d)
	{
		LightManager& lights = GetOwner().GetWindow().GetLights();
		lights.Sun->GetCamera().Draw(d, &GetCamera());
	}
}
