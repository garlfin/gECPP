//
// Created by scion on 10/23/2023.
//

#include "Light.h"

#include <Window.h>
#include <glm/gtx/string_cast.hpp>

#include "DirectionalLight.h"
#include "PointLight.h"

namespace gE
{
	OrthographicCameraSettings CreateDirectionalSettings(u16, float);
	CameraSettings1D CreatePointSettings(u16);

	Light::Light(Window* w, Camera& c, IDepthTarget& d) :
		Entity(w),
		Managed<Light>(&GetWindow().GetLights(), *this),
		_camera(c)
	{
	}

	void Light::GetGPULight(GPU::Light& light) const
	{
		const Transform& transform = GetTransform();
		const Camera& camera = GetCamera();

		light.Depth = GetDepth().GetHandle();
 		light.ViewProjection = camera.GetProjection() * inverse(transform.Model());
		light.Position = transform->Position;
		light.Color = GetColor();
		light.Planes = camera.GetClipPlanes();
	}

	REFLECTABLE_ONGUI_IMPL(Light,
	{
		/*DrawField(Field{ "Camera" }, *_camera, depth);
		DrawField(ScalarField{ "Color", "", 0.01f, FLT_MAX, FLT_EPSILON, ScalarViewMode::ColorPicker }, _color, depth);*/
	});

	void LightManager::OnRender(float delta, Camera* camera)
	{
		for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
			(**i)->GetCamera().OnRender(delta, camera);
	}

	void LightManager::UseSun() const
	{
		DefaultPipeline::Buffers& buffers = _window->GetPipelineBuffers();
		GPU::Lighting& lighting = **buffers.GetLights().GetData();

		Sun->GetGPULight(lighting.Sun);
		buffers.GetLights().UpdateData<GPU::Light>(1, offsetof(GPU::Lighting, Sun));
	}

	DirectionalLight::DirectionalLight(Window* w, u16 size, float scale, const quat& rot) :
		Light(w, _camera, _target),
		_camera(this, _target, CreateDirectionalSettings(size, scale)),
		_target(*this, _camera)
	{
		GetTransform().SetRotation(rot);
		Depth = &_target.GetDepth();
	}

	DirectionalLightTarget::DirectionalLightTarget(Light& light, OrthographicCamera& camera) :
		RenderTarget(light, camera),
		DepthTarget(camera, *_depth)
	{
		DirectionalLightTarget::Resize();
	}

	void DirectionalLightTarget::RenderPass(float, Camera* callingCamera)
	{
		OrthographicCamera& camera = GetCamera();
		Window& window = camera.GetWindow();

		window.RenderState = RenderState::Shadow;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		GetCamera().SetViewport();

		window.GetRenderers().OnRender(0.f, &camera);
	}

	void DirectionalLightTarget::Resize()
	{
		if(_depth->GetSize() == GetSize()) return;

		PlacementNew(_depth, GetFrameBuffer(), GPU::Texture2D(ShadowMapFormat, GetSize()));
	}

	bool DirectionalLightTarget::Setup(float, Camera* callingCamera)
	{
		if(!callingCamera) return false;

		Transform& transform = GetOwner().GetTransform();
		const TransformData& cameraTransform = callingCamera->GetOwner().GetTransform().GetGlobalTransform();

		vec2 planes = GetCamera().GetClipPlanes();
		vec3 offset = -transform->Forward();
		offset *= (planes.y - planes.x) * 0.5 + planes.x;
		//		  (far - near) / 2 + near
		//		  places the middle on the camera

		transform.SetPosition(floor(cameraTransform.Position) + offset);
		transform.OnUpdate(0.f); // Force update on model matrix since it passed its tick.

		return true;
	}

	void DirectionalLight::GetGPULight(GPU::Light& light) const
	{
		Light::GetGPULight(light);

		const Transform& transform = GetTransform();
		const OrthographicCamera& camera = GetCamera();

		light.Position = -transform->Forward();
		light.Type = GPU::LightType::Directional;
		light.PackedSettings = (u32) camera.GetScale().y * 2u;
	}

	REFLECTABLE_FACTORY_NO_IMPL(DirectionalLight);

	PointLightTarget::PointLightTarget(Light& light, CameraCube& camera) :
		RenderTarget(light, camera),
		IDepthTarget(camera, *_depth)
	{
		PointLightTarget::Resize();
	}

	void PointLightTarget::RenderPass(float, Camera* callingCamera)
	{
		CameraCube& camera = GetCamera();
		Window& window = camera->GetWindow();

		window.RenderState = RenderState::ShadowCube;

		glDepthMask(1);
		glColorMask(0, 0, 0, 0);
		glClear(GL_DEPTH_BUFFER_BIT);
		GetCamera().SetViewport();

		window.GetRenderers().OnRender(0.f, &camera);
	}

	void PointLightTarget::Resize()
	{
		PlacementNew(_depth, GetFrameBuffer(), GPU::TextureCube(ShadowMapFormat, GetCamera().GetSize()));
	}

	PointLight::PointLight(Window* window, u16 resolution) : Light(window, _camera, _target),
         _camera(this, _target, CreatePointSettings(resolution)),
         _target(*this, _camera)
    {
		Depth = &_target.GetDepth();
    }

	void PointLight::GetGPULight(GPU::Light& light) const
	{
		Light::GetGPULight(light);

		light.Type = GPU::LightType::Point;
		light.PackedSettings = *(u32*) &_radius;
	}

	REFLECTABLE_ONGUI_IMPL(PointLight,
	{
		//DrawField(ScalarField{ "Radius", "", 0.01f }, _radius, depth);
	});

	REFLECTABLE_FACTORY_NO_IMPL(PointLight);

	OrthographicCameraSettings CreateDirectionalSettings(u16 size, float scale)
	{
		static constexpr ICameraSettings DirectionalSettings
		{
			ClipPlanes(0.1, 25),
			DEFAULT,
		};

		return OrthographicCameraSettings
		{
			CameraSettings2D(DirectionalSettings, ivec2(size)),
			vec2(scale)
		};
	}

	CameraSettings1D CreatePointSettings(u16 size)
	{
		static constexpr ICameraSettings PointSettings
		{
			ClipPlanes(0.1, 5.f),
			DEFAULT,
		};

		return CameraSettings1D
		{
			PointSettings, size
		};
	}
}

