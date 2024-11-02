//
// Created by scion on 9/5/2023.
//

#include "Camera.h"
#include <Engine/Window.h>
#include <Engine/Component/Transform.h>
#include <glm/gtx/string_cast.hpp>

#define NOT(EXPR) (!(EXPR))

CONSTEXPR_GLOBAL glm::vec3 ForwardDirs[]
{
	glm::vec3(1, 0, 0),
	glm::vec3(-1, 0, 0),
	glm::vec3(0, 1, 0),
	glm::vec3(0, -1, 0),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, -1)
};

CONSTEXPR_GLOBAL glm::vec3 UpDirs[]
{
	glm::vec3(0, -1, 0),
	glm::vec3(0, -1, 0),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, -1),
	glm::vec3(0, -1, 0),
	glm::vec3(0, -1, 0)
};


namespace gE
{
	Camera::Camera(Entity* p, TextureSize2D size, IRenderTarget& t, const ICameraSettings& s, ComponentManager<Camera>* m) : Component(
			p, m),
		Projection(),
		_settings(s),
		_target(t),
		_viewportSize(size)
	{
	}

	void Camera::GetGPUCamera(GPU::Camera& cam)
	{
		Transform& transform = GetOwner().GetTransform();

		cam.Position = transform.GetGlobalTransform().Position;
		cam.Frame = Frame;
		cam.Planes = GetClipPlanes();
		cam.Size = _viewportSize;
		cam.Projection = Projection;
		cam.PreviousViewProjection = Projection * inverse(transform.PreviousRenderModel());
		cam.FrameDelta = GetWindow().GetFrameDelta();

		cam.DepthTexture = (handle) 0u;
		cam.ColorTexture = (handle) 0u;
	}

	void Camera::OnRender(float delta, Camera* callingCamera)
	{
		UpdateProjection();

		DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();

		bool isFirst = _settings.Timing.GetIsFirst();
		bool shouldTick = _settings.Timing.Tick(delta);

		if(!isFirst && (!shouldTick || GetOwner().GetFlags().Static)) return;

		// Limits "recursion"
		if(!_target->Setup(delta, callingCamera)) return;
		_target->RenderDependencies(delta);

		GetGPUCamera(buffers.Camera);
		buffers.UpdateCamera();

		_target->Bind();
		_target->RenderPass(delta, callingCamera);
		_target->PostProcessPass(delta);

		Frame++;
	}

	void PerspectiveCamera::UpdateProjection()
	{
		Projection = glm::perspectiveFov(_fov, (float) GetSize().x, (float) GetSize().y, GetClipPlanes().x, GetClipPlanes().y);
	}

	Camera2D::Camera2D(Entity* p, TARGET_TYPE& t, const CameraSettings2D& s, ComponentManager<Camera>* m) :
		Camera(p, s.Size, t, s, m)
	{
	}

	PerspectiveCamera::PerspectiveCamera(Entity* p, TARGET_TYPE& t, const PerspectiveCameraSettings& s, ComponentManager<Camera>* m) :
		Camera2D(p, t, s, m)
	{
		SetFOV(s.FOV);
	}

	void PerspectiveCamera::GetGPUCamera(GPU::Camera& camera)
	{
		Camera2D::GetGPUCamera(camera);
		camera.Parameters.x = GetFOV<AngleType::Radian>();
	}

	OrthographicCamera::OrthographicCamera(Entity* p, TARGET_TYPE& t, const OrthographicCameraSettings& s, ComponentManager<Camera>* m) :
		Camera2D(p, t, s, m), _orthographicScale(s.Scale)
	{
	}

	void OrthographicCamera::UpdateProjection()
	{
		Projection = glm::ortho(_orthographicScale.x, _orthographicScale.y, _orthographicScale.z, _orthographicScale.w, GetClipPlanes().x, GetClipPlanes().y);
	}

	void Camera2D::GetGPUCamera(GPU::Camera& camera)
	{
		Camera::GetGPUCamera(camera);
		camera.View[0] = inverse(GetOwner().GetTransform().Model());
	}

	Camera3D::Camera3D(Entity* p, TARGET_TYPE& t, const CameraSettings3D& s, ComponentManager<Camera>* m) :
		Camera(p, s.Size, t, s, m), _sizeZ(s.Size.z)
	{
	}

	void Camera3D::UpdateProjection()
	{
		glm::vec3 scale = GetOwner().GetTransform()->Scale;
		Projection = glm::ortho(-scale.x, scale.x, -scale.z, scale.z, 0.01f, scale.y * 2.f);
	}

	void Camera3D::GetGPUCamera(GPU::Camera& cam)
	{
		Camera::GetGPUCamera(cam);

		glm::vec3 scale = GetOwner().GetTransform()->Scale;

		for(u8 i = 0; i < 3; i++)
			cam.View[i] = lookAt(cam.Position - scale * ForwardDirs[i * 2], cam.Position, UpDirs[i * 2]);
	}

	CameraCube::CameraCube(Entity* p, TARGET_TYPE& t, const CameraSettings1D& s, ComponentManager<Camera>* m) :
		Camera(p, TextureSize2D(s.Size), t, s, m)
	{
	}

	void CameraCube::UpdateProjection()
	{
		Projection = glm::perspectiveFov(glm::radians(90.f), 1.f, 1.f, GetClipPlanes().x, GetClipPlanes().y);
	}

	void CameraCube::GetGPUCamera(GPU::Camera& cam)
	{
		Camera::GetGPUCamera(cam);

		for(u8 i = 0; i < 6; i++)
			cam.View[i] = lookAt(cam.Position, cam.Position + ForwardDirs[i], UpDirs[i]);
	}
}