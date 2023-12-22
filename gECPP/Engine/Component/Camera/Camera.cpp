//
// Created by scion on 9/5/2023.
//

#include "Camera.h"
#include "Engine/Component/Transform.h"
#include "Engine/Window.h"

#define NOT(EXPR) (!(EXPR))

namespace gE
{
	Camera::Camera(Entity* p, GL::TextureSize2D size, IRenderTarget& t, const ICameraSettings& s) :
		Component(p), _settings(s), _target(t), _viewportSize(size)
	{
	}

	void Camera::GetGLCamera(GL::Camera& cam)
	{
		Transform& transform = GetOwner()->GetTransform();

		cam.Position = transform.GetGlobalTransform().Position;
		cam.Frame = Frame;
		cam.ClipPlanes = GetClipPlanes();
		cam.Size = _viewportSize;
		cam.Projection = Projection;
		cam.PreviousViewProjection = Projection * glm::inverse(transform.PreviousModel());

		cam.DepthTexture = (handle) 0u;
		cam.ColorTexture = (handle) 0u;
	}

	void Camera::Draw(float delta, Camera* callingCamera)
	{
		if(_isProjectionInvalid) UpdateProjection();
		_isProjectionInvalid = false;

		DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();

		bool isFirst = _settings.Timing.GetIsFirst();
		bool shouldTick = _settings.Timing.Tick(delta);

		if(!isFirst && (!shouldTick || GetOwner()->GetFlags().Static)) return;

		// Limits "recursion"
		if(!callingCamera) _target.RenderDependencies(delta);
		if(!_target.Setup(delta, callingCamera)) return;

		GetGLCamera(buffers.Camera);
		buffers.UpdateCamera();

		_target.Bind();
		_target.RenderPass(delta, callingCamera);
		_target.PostProcessPass(delta);

		Frame++;
	}

	void PerspectiveCamera::UpdateProjection()
	{
		Projection = glm::perspectiveFov(_fov, (float) GetSize().x, (float) GetSize().y, GetClipPlanes().x, GetClipPlanes().y);
	}

	Camera2D::Camera2D(Entity* p, TARGET_TYPE& t, const CameraSettings2D& s) :
		Camera(p, s.Size, t, s)
	{
	}

	PerspectiveCamera::PerspectiveCamera(Entity* p, TARGET_TYPE& t, const PerspectiveCameraSettings& s) :
		Camera2D(p, t, s)
	{
		SetFOV(s.FOV);
	}

	OrthographicCamera::OrthographicCamera(Entity* p, TARGET_TYPE& t, const OrthographicCameraSettings& s) :
		Camera2D(p, t, s), _orthographicScale(s.Scale)
	{
	}

	void OrthographicCamera::UpdateProjection()
	{
		Projection = glm::ortho(_orthographicScale.x, _orthographicScale.y, _orthographicScale.z, _orthographicScale.w, GetClipPlanes().x, GetClipPlanes().y);
	}

	void Camera2D::GetGLCamera(GL::Camera& camera)
	{
		Camera::GetGLCamera(camera);
		camera.View[0] = glm::inverse(GetOwner()->GetTransform().Model());
	}

	Camera3D::Camera3D(Entity* p, TARGET_TYPE& t, const CameraSettings3D& s) :
		Camera(p, s.Size, t, s), _sizeZ(s.Size.z)
	{
	}

	void Camera3D::UpdateProjection()
	{
		glm::vec2 halfSize = glm::vec2(GetSize().x, GetSize().z) / 2.f;
		Projection = glm::ortho(-halfSize.x, -halfSize.y, halfSize.x, halfSize.y, GetClipPlanes().x, GetClipPlanes().y);
	}

	void Camera3D::GetGLCamera(GL::Camera& cam)
	{
		Camera::GetGLCamera(cam);

		cam.Projection = Projection;
		cam.View[0] = glm::lookAt(cam.Position, cam.Position + glm::vec3(0, -1, 0), cam.Position + glm::vec3(1, 0, 0));
	}

	CameraCubemap::CameraCubemap(Entity* p, TARGET_TYPE& t, const CameraSettings1D& s) :
		Camera(p, GL::TextureSize2D(s.Size), t, s)
	{
	}

	void CameraCubemap::UpdateProjection()
	{
		Projection = glm::perspectiveFov(glm::radians(90.f), 1.f, 1.f, GetClipPlanes().x, GetClipPlanes().y);
	}

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

	void CameraCubemap::GetGLCamera(GL::Camera& cam)
	{
		Camera::GetGLCamera(cam);

		for(u8 i = 0; i < 6; i++)
			cam.View[i] = glm::lookAt(cam.Position, cam.Position + ForwardDirs[i], UpDirs[i]);
	}
}