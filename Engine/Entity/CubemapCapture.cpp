//
// Created by scion on 10/23/2023.
//

#include "CubemapCapture.h"

#include <Window.h>
#include <Core/Converter/PVR.h>
#include <Entity/Light/DirectionalLight.h>

#define SH_MODE_SAMPLE 0u
#define SH_MODE_MERGE 1u
#define SH_SAMPLE_GROUPS 32

namespace gE
{
	CONSTEXPR_GLOBAL ICameraSettings CubemapCameraSettings
	{
		ClipPlanes(0.1, 100),
		DEFAULT
	};

	CubemapCapture::CubemapCapture(Window* w, u16 size) :
		Entity(w, nullptr, LayerMask::All, EntityFlags(true)),
		_cubemapManaged(&GetWindow().GetCubemaps(), *this),
		_camera(this, _target, { CubemapCameraSettings, size }),
		_target(_camera)
	{
	}

	void CubemapCapture::GetGPUCubemap(GPU::Cubemap& cubemap) const
	{
		const Transform& transform = GetTransform();

		cubemap.Position = transform.GetGlobalTransform().Position;
		cubemap.Scale = transform.GetGlobalTransform().Scale;
		cubemap.BlendRadius = 0.f;
		cubemap.Type = GPU::CubemapType::AABB;
		cubemap.Color = (handle) GetColor();
	}

	void CubemapManager::OnRender(float delta, Camera* camera)
	{
		DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();
		GPU::Lighting& lighting = **buffers.GetLights().GetData();

		GE_ASSERTM((bool) Skybox, "ERROR: NO SKYBOX TEXTURE");

		lighting.Skybox = Skybox->GetHandle();
		buffers.GetLights().UpdateData<std::byte>(sizebetween(GPU::Lighting, Skybox, SkyboxIrradiance), offsetof(GPU::Lighting, Skybox));

		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->GetCamera().OnRender(delta, camera);
	}

	void CubemapManager::LoadSkybox(const Path& path)
	{
		Skybox = ref_cast((GL::TextureCube*) PVR::Read(_window, "Resource/Texture/sky.pvr", GPU::WrapMode::Clamp));
		Skybox->Free();
		CreateHarmonic();
	}

	void CubemapManager::CreateHarmonic() const
	{
		GPU::ComputeShader shaderSettings("Resource/Shader/Compute/sh.comp");
		API::ComputeShader shader(_window, move(shaderSettings));
		shader.Free();

		GPU::Buffer<ColorHarmonic> bufferSettings(SH_SAMPLE_GROUPS, nullptr, sizeof(ColorHarmonic), GPU::BufferUsageHint::Read, true);
		API::Buffer<ColorHarmonic> buf(_window, move(bufferSettings));

		buf.Bind(GL::BufferBaseTarget::ShaderStorage, 9);
		shader.SetUniform(0, Skybox, 0);
		shader.SetUniform(1, SH_MODE_SAMPLE);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_UNIFORM_BARRIER_BIT);
		shader.Dispatch(SH_SAMPLE_GROUPS);

		static_assert(SH_SAMPLE_GROUPS <= 32);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_UNIFORM_BARRIER_BIT);
		shader.SetUniform(1, SH_MODE_MERGE);
		shader.Dispatch(1);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_UNIFORM_BARRIER_BIT);
		buf.RetrieveData();

		_window->GetPipelineBuffers().GetLights().GetData()[0].SkyboxIrradiance = buf.GetData()[0];
	}

	CubemapManager::CubemapManager(Window* window) : Manager(),
		_window(window)
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

		window.RenderState = RenderState::Cubemap;
		camera.GetFlagOverrides(window.RenderState);

		glDepthMask(1);
		glColorMask(1, 1, 1, 1);
		GetCamera().SetViewport();

		glClear(GL_DEPTH_BUFFER_BIT);

		window.GetRenderers().OnRender(0.f, &camera);
		window.GetCubemaps().DrawSkybox();
	}

	void CubemapTarget::Resize()
	{
		if(_depth->GetSize() == GetSize()) return;

		PlacementNew(_depth, GetFrameBuffer(), GPU::TextureCube{ DefaultPipeline::DepthFormat, GetSize() });
		PlacementNew(_color, GetFrameBuffer(), GPU::TextureCube{ CubemapColorFormat, GetSize() });
	}

	CubemapTarget::CubemapTarget(CameraCube& camera) :
		RenderTarget(camera.GetOwner(), camera),
		DepthTarget(camera, _depth.Get())
	{
		CubemapTarget::Resize();
	}

	void CubemapTarget::RenderDependencies(float d)
	{
		LightManager& lights = GetOwner().GetWindow().GetLights();
		lights.Sun->GetCamera().OnRender(d, &RenderTarget::GetCamera());
	}
}
