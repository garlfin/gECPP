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
	Component(parent, m), SizelessCameraSettings(settings), FrameBuffer(&parent->GetWindow())
{
	GE_ASSERT(settings.RenderPass, "RENDERPASS SHOULD NOT BE NULL!");
}

void gE::Camera::OnRender(float delta)
{
	GetWindow().GetCameras().CallingCamera = this;
	if(_isProjectionInvalid) UpdateProjection();
	_isProjectionInvalid = false;

	DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();

	bool isFirst = Timing.GetIsFirst();
	bool shouldTick = Timing.Tick(delta);
	if NOT(isFirst || !GetOwner()->GetFlags().Static && shouldTick) return;

	GetGLCamera(buffers.Camera);
	buffers.UpdateCamera();

	FrameBuffer.Bind();
	RenderPass(&GetWindow(), this);

	for(u8 i = 0; i < GE_MAX_ATTACHMENTS; i++)
		if(AttachmentCopies[i]) AttachmentCopies[i]->CopyFrom(Attachments[i]);

	for(PostProcessPass* pass : PostProcessPasses) pass->Pass(&GetWindow(), this, nullptr);

	Frame++;
}

void gE::Camera::GetGLCamera(GL::Camera& cam)
{
	cam.Position = GetOwner()->GetTransform().GlobalTranslation();
	cam.Frame = Frame;
	cam.ClipPlanes = GetClipPlanes();
	cam.Projection = Projection;
	cam.DepthTexture = (GL::TextureHandle) *GetDepthAttachment();

	if(GL::Texture* t = GetAttachment<0, true>()) cam.ColorTexture = (GL::TextureHandle) *t;
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
void gE::Camera::CreateAttachments(CAM_T& cam, const gE::AttachmentSettings& settings)
{
	if(settings.Depth)
	{
		cam.DepthTexture = (SmartPointer<GL::Texture>)new TEX_T(&cam.GetWindow(), { settings.Depth, cam.GetSize() });

		if constexpr(!std::is_same_v<TEX_T, GL::Texture3D>)
			cam.FrameBuffer.SetDepthAttachment((GL::Texture*) cam.DepthTexture);
	}

	for(u8 i = 0; i < GE_MAX_ATTACHMENTS; i++)
	{
		if(!settings.Attachments[i]) continue;
		cam.Attachments[i] = (SmartPointer<GL::Texture>) new TEX_T(&cam.GetWindow(), { settings.Attachments[i], cam.GetSize() });
		if constexpr(!std::is_same_v<TEX_T, GL::Texture3D>)
			cam.FrameBuffer.SetAttachment(i, (GL::Texture*) cam.Attachments[i]);
	}
}

gE::Camera2D::Camera2D(gE::Entity* parent, Manager* m, const gE::CameraSettings2D& settings) :
	Camera(parent, m, settings), _size(settings.Size)
{
	CreateAttachments<GL::Texture2D>(*this, settings.RenderAttachments);
}

void gE::Camera2D::GetGLCamera(GL::Camera& camera)
{
	Camera::GetGLCamera(camera);
	camera.View[0] = glm::inverse(GetOwner()->GetTransform().Model());
}

gE::Camera3D::Camera3D(gE::Entity* parent, Manager* m, const gE::CameraSettings3D& settings) :
	Camera(parent, m, settings), _size(settings.Size)
{
	CreateAttachments<GL::Texture3D>(*this, settings.RenderAttachments);
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
	CreateAttachments<GL::TextureCube>(*this, settings.RenderAttachments);
}

void gE::CameraCubemap::UpdateProjection()
{
	Projection = glm::perspectiveFov(degree_cast<AngleType::Radian>(90.f), (float) GetSize(), (float) GetSize(), GetClipPlanes().x, GetClipPlanes().y);
}

CONST_GLOBAL glm::vec3 ForwardDirs[]
{
	glm::vec3(1, 0, 0),
	glm::vec3(-1, 0, 0),
	glm::vec3(0, 1, 0),
	glm::vec3(0, -1, 0),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0, -1)
};

CONST_GLOBAL glm::vec3 UpDirs[]
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
