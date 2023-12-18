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
		_camera(this, nullptr, _target, CreateDirectionalSettings(size, scale)),
		_target(*this, _camera)
	{
		GetTransform().Rotation = rot;
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

	Light::Light(Window* w, Camera& c, IDepthTarget& t, Entity* p) : Entity(w, Flags(), p),
		_camera(c), _target(t)
	{
	}

	void LightManager::OnRender(float delta)
	{
		TypedManager<Light>::OnRender(delta);
	}

	CONSTEXPR_GLOBAL GL::ITextureSettings ShadowMapFormat { GL_DEPTH_COMPONENT16, GL::WrapMode::Clamp, GL::FilterMode::Linear };

	DirectionalLightTarget::DirectionalLightTarget(Light& light, OrthographicCamera& camera) :
		RenderTarget<Camera2D>(light, camera), IDepthTarget((GL::Texture&) _depth),
		_depth(GetFrameBuffer(), GL::TextureSettings2D(ShadowMapFormat, camera.GetSize()))
	{

	}

	void DirectionalLightTarget::RenderPass(float, Camera* callingCamera)
	{
		if(!callingCamera) return;

		Window& window = GetWindow();
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

	void DirectionalLightTarget::Setup(float, Camera* callingCamera)
	{
		Window& window = GetWindow();
		OrthographicCamera& camera = GetCamera();
		GL::TextureSize2D size = camera.GetSize();

		Transform& transform = camera.GetOwner()->GetTransform();
		Transform& cameraTransform = callingCamera->GetOwner()->GetTransform();

		glm::vec2 planes = GetCamera().GetClipPlanes();
		glm::vec3 offset = -transform.Forward();
		offset *= (planes.y - planes.x) * 0.5 + planes.x;
		//		  (far - near) / 2 + near
		//		  places the middle on the camera

		transform.Position = glm::floor(cameraTransform.Position) + offset;
		transform.OnRender(0.f); // Force update on model matrix since it passed its tick.
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

	IDepthTarget::IDepthTarget(GL::Texture& d) : _depth(d)
	{
	}
}

