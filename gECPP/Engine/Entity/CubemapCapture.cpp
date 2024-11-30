//
// Created by scion on 10/23/2023.
//

#include "CubemapCapture.h"
#include <Engine/Window/Window.h>
#include <Engine/Entity/Light/DirectionalLight.h>

namespace gE
{
	CONSTEXPR_GLOBAL ICameraSettings CubemapCameraSettings
	{
		ClipPlanes(0.1, 100),
		DefaultCameraTiming
	};

	CubemapCapture::CubemapCapture(Window* w, u16 size) :
		Entity(w, nullptr, LayerMask::All, EntityFlags(true)),
		_cubemapManaged(&GetWindow().GetCubemaps(), *this),
		_camera(this, _target, { CubemapCameraSettings, size }),
		_target(_camera)
	{}

	void CubemapCapture::GetGPUCubemap(GPU::Cubemap& cubemap)
	{
		Transform& transform = GetTransform();

		cubemap.Position = transform.GetGlobalTransform().Position;
		cubemap.Scale = transform.GetGlobalTransform().Scale;
		cubemap.BlendRadius = 0.f;
		cubemap.Type = GPU::CubemapType::AABB;
		cubemap.Color = (handle) GetColor();
	}

	void CubemapManager::OnRender(float delta, Camera* camera)
	{
		DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();
		GPU::Lighting& lighting = buffers.Lighting;

		GE_ASSERT(Skybox.Get(), "ERROR: NO SKYBOX TEXTURE");

		lighting.Skybox = Skybox->GetHandle();
		buffers.UpdateLighting(sizeof(handle), offsetof(GPU::Lighting, Skybox));

		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->GetCamera().OnRender(delta, camera);
	}

	void CubemapManager::UseNearestCubemaps(const glm::vec3& point) const
	{
		DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();
		GPU::Lighting& lighting = buffers.Lighting;

		lighting.CubemapCount = 1;
		(**List.GetFirst())->GetGPUCubemap(lighting.Cubemaps[0]);

		buffers.UpdateLighting(sizeof(u32), offsetof(GPU::Lighting, CubemapCount));
		buffers.UpdateLighting(sizeof(GPU::Cubemap), offsetof(GPU::Lighting, Cubemaps));
	}

	CubemapManager::CubemapManager(Window* window): Manager(), _window(window)
	{
		ReadSerializableFromFile(_window, "Resource/Model/skybox.vao", _skyboxVAO);

		GPU::Shader shaderSettings("Resource/Shader/skybox.vert", "Resource/Shader/skybox.frag");
		_skyboxShader = move(API::Shader(window, move(shaderSettings)));
		_skyboxShader.Free();
	}

	void CubemapManager::DrawSkybox() const
	{
		_skyboxShader.Bind();

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL);

		_skyboxVAO.Draw(0, _window->RenderState.InstanceMultiplier);
	}

	void CubemapTarget::RenderPass(float, Camera*)
	{
		CameraCube& camera = GetCamera();
		Window& window = camera.GetWindow();

		window.GetLights().UseNearestLights(glm::vec3(0.0f));

		window.RenderState = RenderState::Cubemap;

		glDepthMask(1);
		glColorMask(1, 1, 1, 1);
		glViewport(0, 0, GetSize(), GetSize());
		glClear(GL_DEPTH_BUFFER_BIT);

		window.GetRenderers().OnRender(0.f, &camera);
		window.GetCubemaps().DrawSkybox();
	}

	GLOBAL GPU::Texture CubemapColorFormat = []
	{
		GPU::Texture tex;
		tex.Format = GL_R11F_G11F_B10F;
		tex.WrapMode = GPU::WrapMode::Clamp;
		return tex;
	}();

	CubemapTarget::CubemapTarget(CameraCube& camera) :
		RenderTarget(camera.GetOwner(), camera), IDepthTarget(_depth.Get()),
		_depth(GetFrameBuffer(), GPU::TextureCube{ DefaultPipeline::DepthFormat, camera.GetSize() }),
		_color(GetFrameBuffer(), GPU::TextureCube{ CubemapColorFormat, camera.GetSize() })
	{
	}

	void CubemapTarget::RenderDependencies(float d)
	{
		LightManager& lights = GetOwner().GetWindow().GetLights();
		lights.Sun->GetCamera().OnRender(d, &GetCamera());
	}
}
