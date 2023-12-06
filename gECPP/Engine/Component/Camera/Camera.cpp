//
// Created by scion on 9/5/2023.
//

#include "Camera.h"
#include "Engine/Component/Transform.h"
#include "Engine/Window.h"

#define NOT(EXPR) (!(EXPR))

namespace gE
{

	Camera::Camera(Entity* p, Manager* m, IRenderTarget& t, const ICameraSettings& s) :
		Component(p, m), _settings(s), _target(t)
	{
	}

	void Camera::OnRender(float delta)
	{
		GetWindow().GetCameras().CallingCamera = this;
		if(_isProjectionInvalid) UpdateProjection();
		_isProjectionInvalid = false;

		DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();

		bool isFirst = _settings.Timing.GetIsFirst();
		bool shouldTick = _settings.Timing.Tick(delta);
		if NOT(isFirst || !GetOwner()->GetFlags().Static && shouldTick) return;

		GetGLCamera(buffers.Camera);
		buffers.UpdateCamera();

		_target.Bind();
		_target.RenderPass();
		_target.PostProcessPass();

		Frame++;
	}

	void Camera::GetGLCamera(GL::Camera& cam)
	{
		Transform& transform = GetOwner()->GetTransform();

		cam.Position = transform.GlobalTranslation();
		cam.Frame = Frame;
		cam.ClipPlanes = GetClipPlanes();

		if(Frame)
			cam.PreviousViewProjection = Projection * glm::inverse(transform.PreviousModel());
		else
			cam.PreviousViewProjection = Projection * glm::inverse(transform.Model());

		cam.Projection = Projection;

		// TODO: FIX AFTER REFACTORING
		cam.DepthTexture = 0; // (handle) *GetDepth();
		cam.ColorTexture = 0; // GetColorCopy() ? ((handle) *GetColorCopy()) : 0;
	}

	void PerspectiveCamera::UpdateProjection()
	{
		Projection = glm::perspectiveFov(_fov, (float) GetSize().x, (float) GetSize().y, GetClipPlanes().x, GetClipPlanes().y);
	}

	Camera2D::Camera2D(Entity* p, Manager* m, TARGET_TYPE& t, const CameraSettings2D& s) :
		Camera(p, m, t, s), _size(s.Size)
	{
	}

	PerspectiveCamera::PerspectiveCamera(Entity* p, Manager* m, TARGET_TYPE& t, const PerspectiveCameraSettings& s) :
		Camera2D(p, m, t, s)
	{
		SetFOV(s.FOV);
	}

	OrthographicCamera::OrthographicCamera(Entity* p, Manager* m, TARGET_TYPE& t, const OrthographicCameraSettings& s) :
		Camera2D(p, m, t, s), _orthographicScale(s.Scale)
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
		camera.Size = GetSize();
	}

	Camera3D::Camera3D(Entity* p, Manager* m, TARGET_TYPE& t, const CameraSettings3D& s) :
		Camera(p, m, t, s), _size(s.Size)
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

	CameraCubemap::CameraCubemap(Entity* p, Manager* m, TARGET_TYPE& t, const CameraSettings1D& s) :
		Camera(p, m, t, s), _size(s.Size)
	{
	}

	void CameraCubemap::UpdateProjection()
	{
		Projection = glm::perspectiveFov(degree_cast<AngleType::Radian>(90.f), (float) GetSize(), (float) GetSize(), GetClipPlanes().x, GetClipPlanes().y);
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
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 0, 1),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 1, 0),
		glm::vec3(0, 1, 0)
	};

	void CameraCubemap::GetGLCamera(GL::Camera& cam)
	{
		Camera::GetGLCamera(cam);

		cam.Projection = Projection;
		for(u8 i = 0; i < 6; i++)
			cam.View[i] = glm::lookAt(cam.Position, cam.Position + ForwardDirs[i], cam.Position + UpDirs[i]);
	}
}