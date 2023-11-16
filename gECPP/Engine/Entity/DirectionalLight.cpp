//
// Created by scion on 10/23/2023.
//

#include "DirectionalLight.h"
#include <Engine/Window.h>

#include <glm/gtx/string_cast.hpp>

namespace gE
{
	const SizelessCameraSettings DirectionalSettings
	{
		(RenderPass) DefaultPipeline::RenderPassShadow,
		ClipPlanes(0.1, 100),
		DefaultCameraTiming,
		DefaultPipeline::AttachmentShadow
	};

	DirectionalLight::DirectionalLight(Window* w, u16 size, float scale) : Light(w, _camera),
		_camera(this, &w->GetLights(), { { DirectionalSettings, glm::u32vec2(size) }, glm::vec4(-scale, scale, -scale, scale) })
	{
	}

	void DirectionalLight::OnRender(float delta)
	{
		// Right now this system is only set up for one camera, but that's ok :)
		GE_ASSERT(GetWindow().GetCameras().CurrentCamera, "CAMERA SHOULD NOT BE NULL");

		Entity* camera = GetWindow().GetCameras().CurrentCamera->GetOwner();
		Transform& transform = GetTransform();
		Transform& cameraTransform = camera->GetTransform();

		glm::vec2 planes = _camera.GetClipPlanes();
		glm::vec3 offset = -transform.Forward();
		offset *= (planes.y - planes.x) * 0.5 + planes.x;
		//		  (far - near) / 2 + near
		//		  places the middle on the camera

		transform.Position = cameraTransform.Position + offset;
		transform.OnRender(0.f); // Force update on model matrix since it passed its tick.
	}

	void DirectionalLight::GetGLLight(GL::LightData& light)
	{
		light.Type = GL::LightType::Directional;
		light.Color = glm::vec3(1);
		light.ViewProjection = _camera.GetProjection();
		light.Depth = (GL::TextureHandle) *GetDepth();

		LOG(light.Depth);
	}

	// Registered to behaviors for tick functionality.
	Light::Light(Window* window, Camera& camera, Entity* parent) : Entity(window, parent, &window->GetBehaviors()),
		_camera(camera)
	{

	}

	void LightManager::OnRender(float delta)
	{
		DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();

		Sun->GetGLLight(buffers.Lighting.Lights[0]);
		buffers.UpdateLighting(offsetof(GL::Lighting, Lights[1]));
	}
}


