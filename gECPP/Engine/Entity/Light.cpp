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

	DirectionalLight::DirectionalLight(Window* w, u16 size, float scale, const glm::quat& rot) :
		Light(w, _camera, _target),
		_camera(this, _target, CreateDirectionalSettings(size, scale)),
		_target(*this, _camera)
	{
		GetTransform().Rotation = rot;
	}

	void LightManager::OnRender(float delta)
	{
		for(Light* l : *this) l->GetCamera().OnRender(delta);
	}

	CONSTEXPR_GLOBAL GL::ITextureSettings ShadowMapFormat { GL_DEPTH_COMPONENT16, GL::WrapMode::Clamp, GL::FilterMode::Linear };

	DirectionalLightTarget::DirectionalLightTarget(Light& l, OrthographicCamera& c) :
		RenderTarget<Camera2D>(l, c), IDepthTarget((GL::Texture&) _depth),
		_depth(GetFrameBuffer(), GL::TextureSettings2D{ ShadowMapFormat, c.GetSize() })
	{
	}

	void DirectionalLightTarget::RenderPass(float, Camera* callingCamera)
	{
		Window& window = callingCamera->GetWindow();
		OrthographicCamera& camera = GetCamera();
		GL::TextureSize2D size = camera.GetSize();

		DefaultPipeline::Buffers& buffers = window.GetPipelineBuffers();

		GetOwner().GetGLLight(buffers.Lighting.Lights[0]);
		buffers.UpdateLighting(offsetof(GL::Lighting, Lights[1]));

		window.State = State::Shadow;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, size.x, size.y);

		window.GetRenderers().OnRender(0.f);
	}

	bool DirectionalLightTarget::Setup(float, Camera* callingCamera)
	{
		if(!callingCamera) return false;

		OrthographicCamera& camera = GetCamera();
		GL::TextureSize2D size = camera.GetSize();

		Transform& transform = GetOwner().GetTransform();
		Transform& cameraTransform = callingCamera->GetOwner()->GetTransform();

		glm::vec2 planes = GetCamera().GetClipPlanes();
		glm::vec3 offset = -transform.Forward();
		offset *= (planes.y - planes.x) * 0.5 + planes.x;
		//		  (far - near) / 2 + near
		//		  places the middle on the camera

		transform.Position = glm::floor(cameraTransform.Position) + offset;
		transform.OnRender(0.f); // Force update on model matrix since it passed its tick.

		return true;
	}

	void DirectionalLight::GetGLLight(GL::Light& light)
	{
		Transform& transform = GetCamera().GetOwner()->GetTransform();
		OrthographicCamera& camera = GetCamera();

		light.ViewProjection = camera.GetProjection() * glm::inverse(transform.Model());
		light.Position = -transform.Forward();
		light.Type = GL::LightType::Directional;
		light.Color = glm::vec3(1);
		light.PackedSettings = u32(camera.GetScale().y * 2);
		light.Planes = DirectionalSettings.ClipPlanes;
		light.Depth = (handle) GetDepth();
	}

	OrthographicCameraSettings CreateDirectionalSettings(u16 size, float scale)
	{
		scale *= 0.5f;

		return OrthographicCameraSettings
		{
			CameraSettings2D(DirectionalSettings, glm::ivec2(size)),
			glm::vec4(-scale, scale, -scale, scale)
		};
	}

	Light::Light(Window* w, Camera& c, IDepthTarget& d) :
		Entity(w, Flags(false, UINT8_MAX)),
		IDepthTarget(d),
		Managed<Light>(*this, GetWindow().GetLights()),
		_camera(c)
	{
	}
}

