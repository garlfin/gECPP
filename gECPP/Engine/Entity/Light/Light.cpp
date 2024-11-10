//
// Created by scion on 10/23/2023.
//

#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include <Engine/Window.h>
#include <glm/gtx/string_cast.hpp>

namespace gE
{
	OrthographicCameraSettings CreateDirectionalSettings(u16, float);
	CameraSettings1D CreatePointSettings(u16);

	Light::Light(Window* w, Camera& c, IDepthTarget& d) :
		Entity(w),
		IDepthTarget(d),
		_managedLight(&GetWindow().GetLights(), *this),
		_camera(c)
	{
	}

	void LightManager::OnRender(float delta, Camera* camera)
	{
		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->GetCamera().OnRender(delta, camera);
	}

	void LightManager::UseNearestLights(const glm::vec3& point) const
	{
		DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();
		GPU::Lighting& lighting = buffers.Lighting;

		lighting.LightCount = 2;
		(**List.GetFirst())->GetGPULight(lighting.Lights[0]);
		(**List.GetFirst()->GetNext())->GetGPULight(lighting.Lights[1]);

		buffers.UpdateLighting(sizeof(u32));
		buffers.UpdateLighting(sizeof(GPU::Light) * 2, offsetof(GPU::Lighting, Lights));
	}

	DirectionalLight::DirectionalLight(Window* w, u16 size, float scale, const glm::quat& rot) :
		Light(w, _camera, _target),
		_camera(this, _target, CreateDirectionalSettings(size, scale)),
		_target(*this, _camera)
	{
		GetTransform().SetRotation(rot);
	}

	DirectionalLightTarget::DirectionalLightTarget(Light& l, OrthographicCamera& c) :
		RenderTarget(l, c), IDepthTarget((API::Texture&) _depth),
		_depth(GetFrameBuffer(), GPU::TextureSettings2D(ShadowMapFormat, c.GetSize()))
	{
	}

	void DirectionalLightTarget::RenderPass(float, Camera* callingCamera)
	{
		OrthographicCamera& camera = GetCamera();
		Window& window = camera.GetWindow();
		TextureSize2D size = camera.GetViewportSize();

		window.RenderState = RenderState::Shadow;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, size.x, size.y);

		window.GetRenderers().OnRender(0.f, &camera);
	}

	bool DirectionalLightTarget::Setup(float, Camera* callingCamera)
	{
		if(!callingCamera) return false;

		OrthographicCamera& camera = GetCamera();

		Transform& transform = GetOwner().GetTransform();
		Transform& cameraTransform = callingCamera->GetOwner().GetTransform();

		glm::vec2 planes = GetCamera().GetClipPlanes();
		glm::vec3 offset = -transform->Forward();
		offset *= (planes.y - planes.x) * 0.5 + planes.x;
		//		  (far - near) / 2 + near
		//		  places the middle on the camera

		transform.SetPosition(floor(cameraTransform->Position) + offset);
		transform.OnUpdate(0.f); // Force update on model matrix since it passed its tick.

		return true;
	}

	void DirectionalLight::GetGPULight(GPU::Light& light)
	{
		Transform& transform = GetTransform();
		OrthographicCamera& camera = GetCamera();

		light.ViewProjection = camera.GetProjection() * inverse(transform.Model());
		light.Position = -transform->Forward();
		light.Type = GPU::LightType::Directional;
		light.Color = glm::vec3(1);
		light.PackedSettings = u32(camera.GetScale().y * 2);
		light.Planes = camera.GetClipPlanes();
		light.Depth = (handle) GetDepth();
	}

	PointLightTarget::PointLightTarget(Light& light, CameraCube& camera) :
		RenderTarget(light, camera),
		IDepthTarget((GL::TextureCube&) _depth),
		_depth(GetFrameBuffer(), GPU::TextureSettings1D(ShadowMapFormat, camera.GetSize()))
	{

	}

	void PointLightTarget::RenderPass(float, Camera* callingCamera)
	{
		CameraCube& camera = GetCamera();
		Window& window = camera->GetWindow();
		TextureSize2D size = camera.GetViewportSize();

		window.RenderState = RenderState::ShadowCube;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, size.x, size.y);

		window.GetRenderers().OnRender(0.f, &camera);
	}

    PointLight::PointLight(Window* window, u16 resolution) : Light(window, _camera, _target),
		_camera(this, _target, CreatePointSettings(resolution)),
		_target(*this, _camera)
    {

    }

	void PointLight::GetGPULight(GPU::Light& light)
	{
		Transform& transform = GetTransform();
		CameraCube& camera = GetCamera();

		light.ViewProjection = camera.GetProjection() * inverse(transform.Model());
		light.Position = transform->Position;
		light.Type = GPU::LightType::Point;
		light.Color = glm::vec3(1.0);
		light.PackedSettings = std::bit_cast<u32, float>(0.1);
		light.Planes = camera.GetClipPlanes();
		light.Depth = (handle) GetDepth();
	}

	OrthographicCameraSettings CreateDirectionalSettings(u16 size, float scale)
	{
		static constexpr ICameraSettings DirectionalSettings
		{
			ClipPlanes(0.1, 25),
			DefaultCameraTiming,
		};

		scale *= 0.5f;

		return OrthographicCameraSettings
		{
			CameraSettings2D(DirectionalSettings, glm::ivec2(size)),
			glm::vec4(-scale, scale, -scale, scale)
		};
	}

	CameraSettings1D CreatePointSettings(u16 size)
	{
		static constexpr ICameraSettings PointSettings
		{
			ClipPlanes(0.1, 5.f),
			DefaultCameraTiming,
		};

		return CameraSettings1D
		{
			PointSettings, size
		};
	}
}

