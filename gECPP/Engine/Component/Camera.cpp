//
// Created by scion on 9/5/2023.
//
#define GLM_FORCE_SWIZZLE
#include <GLM/glm.hpp>
#include "Camera.h"
#include "Engine/Component/Transform.h"
#include "Engine/Window.h"

#define NOT(EXPR) (!(EXPR))

gE::Camera::Camera(gE::Entity* parent, Manager* m, const SizelessCameraSettings& settings) :
	Component(parent, m), _settings(settings), FrameBuffer(&parent->GetWindow())
{
	// Consolidate requirements
	for(PostProcessEffect* effect : settings.PostProcessEffects)
		_settings.Attachments |= effect->GetRequirements();

	GE_ASSERT(settings.RenderPass, "RENDERPASS SHOULD NOT BE NULL!");
}

void gE::Camera::OnRender(float delta)
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

	FrameBuffer.Bind();
	_settings.RenderPass(*this);

	GL::Texture* in = GetColor();
	GL::Texture* out = GetColorCopy();

	for(uint i = 0; i < _settings.PostProcessEffects.size(); i++)
	{
		std::swap(in, out);
		_settings.PostProcessEffects[i]->RenderPass(*this, *in, *out);
	}

	if(in && out) in->CopyFrom(*out);

	Frame++;
}

void gE::Camera::GetGLCamera(GL::Camera& cam)
{
	cam.Position = GetOwner()->GetTransform().GlobalTranslation();
	cam.Frame = Frame;
	cam.ClipPlanes = GetClipPlanes();
	cam.Projection = Projection;
	cam.DepthTexture = (GL::TextureHandle) *GetDepth();
	cam.ColorTexture = GetColorCopy() ? ((GL::TextureHandle) *GetColorCopy()) : 0;
}

void gE::PerspectiveCamera::UpdateProjection()
{
	Projection = glm::perspectiveFov(_fov, (float) GetSize().x, (float) GetSize().y, GetClipPlanes().x, GetClipPlanes().y);
}

gE::PerspectiveCamera::PerspectiveCamera(gE::Entity* e, Manager* m, const gE::PerspectiveCameraSettings& s)
	: Camera2D(e, m, s)
{
	SetFOV(s.FOV);
}

void gE::OrthographicCamera::UpdateProjection()
{
	Projection = glm::ortho(_orthographicScale.x, _orthographicScale.y, _orthographicScale.z, _orthographicScale.w, GetClipPlanes().x, GetClipPlanes().y);
}

gE::OrthographicCamera::OrthographicCamera(gE::Entity* e, Manager* m, const gE::OrthographicCameraSettings& s) :
	Camera2D(e, m, s), _orthographicScale(s.Scale)
{
}

template<class TEX_T, class CAM_T>
void gE::Camera::CreateAttachments(CAM_T& cam)
{
	const SizelessCameraSettings& settings = cam.GetSettings();
	const AttachmentSettings& attachments = cam.GetAttachmentSettings();

	if(settings.Depth)
	{
		cam.Depth = (SmartPointer<GL::Texture>) new TEX_T(&cam.GetWindow(), { settings.Depth, cam.GetSize() });

		if constexpr(!std::is_same_v<TEX_T, GL::Texture3D>)
			cam.FrameBuffer.SetDepthAttachment((GL::Texture*) cam.Depth);
	}

	for(u8 i = 0; i < GE_MAX_ATTACHMENTS; i++)
	{
		if(!attachments.Attachments[i]) continue;
		cam.Attachments[i] = (SmartPointer<GL::Texture>) new TEX_T(&cam.GetWindow(), { attachments.Attachments[i], cam.GetSize() });

		if constexpr(!std::is_same_v<TEX_T, GL::Texture3D>)
			cam.FrameBuffer.SetAttachment(i, (GL::Texture*) cam.Attachments[i]);
	}
}

gE::Camera2D::Camera2D(gE::Entity* parent, Manager* m, const gE::CameraSettings2D& settings) :
	Camera(parent, m, settings), _size(settings.Size)
{
	CreateAttachments<GL::Texture2D>(*this);
}

void gE::Camera2D::GetGLCamera(GL::Camera& camera)
{
	Camera::GetGLCamera(camera);
	camera.View[0] = glm::inverse(GetOwner()->GetTransform().Model());
}

gE::Camera3D::Camera3D(gE::Entity* parent, Manager* m, const gE::CameraSettings3D& settings) :
	Camera(parent, m, settings), _size(settings.Size)
{
	CreateAttachments<GL::Texture3D>(*this);
}

void gE::Camera3D::UpdateProjection()
{
	glm::vec2 halfSize = (glm::vec2) GetSize().xz() / 2.f;
	Projection = glm::ortho(-halfSize.x, -halfSize.y, halfSize.x, halfSize.y, GetClipPlanes().x, GetClipPlanes().y);
}

void gE::Camera3D::GetGLCamera(GL::Camera& cam)
{
	Camera::GetGLCamera(cam);

	cam.Projection = Projection;
	cam.View[0] = glm::lookAt(cam.Position, cam.Position + glm::vec3(0, -1, 0), cam.Position + glm::vec3(1, 0, 0));
}

gE::CameraCubemap::CameraCubemap(gE::Entity* parent, Manager* m, const gE::CameraSettings1D& settings) :
	Camera(parent, m, settings), _size(settings.Size)
{
	CreateAttachments<GL::TextureCube>(*this);
}

void gE::CameraCubemap::UpdateProjection()
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

void gE::CameraCubemap::GetGLCamera(GL::Camera& cam)
{
	Camera::GetGLCamera(cam);

	cam.Projection = Projection;
	for(u8 i = 0; i < 6; i++)
		cam.View[i] = glm::lookAt(cam.Position, cam.Position + ForwardDirs[i], cam.Position + UpDirs[i]);
}

gE::PostProcessEffect::PostProcessEffect(Window* w, const AttachmentSettings& s) :
	_window(w), _requirements(s)
{
}


