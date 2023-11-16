//
// Created by scion on 10/23/2023.
//

#include "DirectionalLight.h"
#include <Engine/Window.h>

namespace gE
{
	const SizelessCameraSettings DirectionalSettings
	{
		(RenderPass) DefaultPipeline::RenderPassDirectionalShadow,
		ClipPlanes(0.1, 100),
		DefaultCameraTiming,
		DefaultPipeline::AttachmentShadow
	};

	DirectionalLight::DirectionalLight(Window* w, u16 size, float scale) :
		OrthographicCamera(w, { { DirectionalSettings, glm::u32vec2(size) }, glm::vec4(scale) }, nullptr, &w->GetLights())
	{
	}

	void DirectionalLight::OnRender(float delta)
	{
		// Right now this system is only set up for one camera, but that's ok : )
		GE_ASSERT(GetWindow().GetCameras().CurrentCamera, "CAMERA SHOULD NOT BE NULL");

		Camera* camera = GetWindow().GetCameras().CurrentCamera;
		Transform& transform = GetTransform();
		Transform& cameraTransform = camera->GetTransform();

		glm::vec2 planes = camera->GetClipPlanes();
		glm::vec3 offset = -transform.Forward();
		offset *= (planes.y - planes.x) * 0.5 + planes.x;
		//		  (far - near) / 2 + near
		//		  places the middle on the camera

		transform.Position = cameraTransform.Position + offset;

		OrthographicCamera::OnRender(delta);
	}
}