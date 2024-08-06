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
		Managed<CubemapCapture>(*this, &GetWindow().GetCubemaps()),
		_camera(this, _target, { CubemapCameraSettings, size }),
		_target(_camera)
	{}

	void CubemapCapture::GetGLCubemap(API::Cubemap& cubemap)
	{
		Transform& transform = GetTransform();

		cubemap.Position = transform.GetGlobalTransform().Position;
		cubemap.Scale = transform.GetGlobalTransform().Scale;
		cubemap.BlendRadius = 0.f;
		cubemap.Type = API::CubemapType::AABB;
		cubemap.Color = (handle) GetColor();
	}

	void CubemapManager::OnRender(float delta, Camera* camera)
	{
		DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();
		API::Lighting& lighting = buffers.Lighting;

		lighting.Skybox = Skybox->GetHandle();
		lighting.CubemapCount = 1;
		(*List.GetFirst())->GetGLCubemap(lighting.Cubemaps[0]);

		buffers.UpdateLighting(sizeof(handle), offsetof(API::Lighting, Skybox));
		buffers.UpdateLighting(sizeof(API::Cubemap), offsetof(API::Lighting, Cubemaps[0]));

		for(Managed<CubemapCapture>* c = List.GetFirst(); c; c = c->GetNext())
			(*c)->GetCamera().OnRender(delta, camera);
	}

	void CubemapManager::DrawSkybox()
	{
		if(!_skyboxVAO)
		{
			gETF::Mesh skybox;
			ReadSerializableFromFile(_window, "Resource/Model/skybox.gEMesh", skybox);

			_skyboxVAO = std::move(skybox.VAO);
			_skyboxShader = ptr_create<API::Shader>(_window, "Resource/Shader/skybox.vert", "Resource/Shader/skybox.frag");
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

		window.GetRenderers().OnRender(0.f, &camera);
		window.GetCubemaps().DrawSkybox();
	}

	CONSTEXPR_GLOBAL API::ITextureSettings CubemapColorFormat
	{
		GL_R11F_G11F_B10F,
		API::WrapMode::Clamp,
		API::FilterMode::Linear
	};

	CubemapTarget::CubemapTarget(CameraCubemap& camera) :
		RenderTarget<CameraCubemap>(*camera.GetOwner(), camera), IDepthTarget(_depth.Get()),
		_depth(GetFrameBuffer(), API::TextureSettings1D{ DefaultPipeline::DepthFormat, camera.GetSize() }),
		_color(GetFrameBuffer(), API::TextureSettings1D{ CubemapColorFormat, camera.GetSize() })
	{
	}

	void CubemapTarget::RenderDependencies(float d)
	{
		LightManager& lights = GetOwner().GetWindow().GetLights();
		lights.Sun->GetCamera().OnRender(d, &GetCamera());
	}
}
