//
// Created by scion on 10/23/2023.
//

#include "Light.h"
#include <Engine/Window.h>

#include <glm/gtx/string_cast.hpp>

namespace gE
{
	const ICameraSettings DirectionalSettings
	{
		ClipPlanes(0.1, 25),
		DefaultCameraTiming,
	};

	OrthographicCameraSettings CreateDirectionalSettings(u16, float);

	DirectionalLight::DirectionalLight(Window* w, u16 size, float scale, const glm::quat& rot) : Light(w, _camera, _target),
		_camera(this, &w->GetLights(), _target, CreateDirectionalSettings(size, scale)),
		_target(_camera)
	{
		GetTransform().Rotation = rot;
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

		transform.Position = glm::floor(cameraTransform.Position) + offset;
		transform.OnRender(0.f); // Force update on model matrix since it passed its tick.
	}

	void DirectionalLight::GetGLLight(GL::LightData& light)
	{
		light.ViewProjection = _camera.GetProjection() * glm::inverse(GetTransform().Model());
		light.Position = -GetTransform().Forward();
		light.Type = GL::LightType::Directional;
		light.Color = glm::vec3(1);
		light.PackedSettings = u32(GetScale() * 2);
		light.Planes = DirectionalSettings.ClipPlanes;
		light.Depth = (handle) GetDepth();
	}

	// Registered to behaviors for tick functionality.
	Light::Light(Window* w, Camera& c, IDepthTarget& t, Entity* p) : Entity(w, p, &w->GetBehaviors()),
		_camera(c), _target(t)
	{
	}

	void LightManager::OnRender(float delta)
	{
		TypedManager<Light>::OnRender(delta);

		DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();

		Sun->GetGLLight(buffers.Lighting.Lights[0]);
		buffers.UpdateLighting(offsetof(GL::Lighting, Lights[1]));
	}

	CONSTEXPR_GLOBAL GL::ITextureSettings ShadowMapFormat { GL_DEPTH_COMPONENT16, GL::WrapMode::Clamp, GL::FilterMode::Linear };

	DirectionalShadowTarget::DirectionalShadowTarget(OrthographicCamera& camera) : RenderTarget<Camera2D>(camera), IDepthTarget(GetFrameBuffer(), _depth),
		_depth(&camera.GetWindow(), GL::TextureSettings2D(ShadowMapFormat, camera.GetSize()))
	{
	}

	void DirectionalShadowTarget::RenderPass()
	{
		OrthographicCamera& camera = GetCamera();
		Window& window = camera.GetWindow();
		GL::TextureSize2D size = camera.GetSize();

		window.Stage = RenderStage::Shadow;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, size.x, size.y);

		window.GetRenderers().OnRender(0.f);
	}

	OrthographicCameraSettings CreateDirectionalSettings(u16 size, float scale)
	{
		return OrthographicCameraSettings
		{
			CameraSettings2D(DirectionalSettings, glm::ivec2(size)),
			glm::vec4(-scale, scale, -scale, scale)
		};
	}

	IDepthTarget::IDepthTarget(GL::FrameBuffer& f, GL::Texture& d) : _depth(d)
	{
		f.SetDepthAttachment(d);
	}
}

