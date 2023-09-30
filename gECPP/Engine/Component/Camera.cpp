//
// Created by scion on 9/5/2023.
//
#define GLM_FORCE_SWIZZLE
#include <GLM/glm.hpp>
#include "Camera.h"
#include "Engine/Component/Transform.h"
#include "Engine/Window.h"


gE::Camera::Camera(gE::Entity* parent, const SizelessCameraSettings& settings) :
	Component(parent), Settings(settings), FrameBuffer(parent->GetWindow())
{
	GE_ASSERT(settings.RenderPass, "RENDERPASS SHOULD NOT BE NULL!");
}

void gE::Camera::OnRender(float delta)
{
	View = glm::inverse(Owner()->GetTransform().Model());
	if(_invalidated) UpdateProjection();
	_invalidated = false;

	Window* window = Owner()->GetWindow();
	DefaultPipeline::Buffers* buffers = window->GetPipelineBuffers();

	GetGLCamera(buffers->Camera);
	buffers->UpdateCamera();

	FrameBuffer.Bind();
	Settings.RenderPass(window, this);
}

gE::Camera::~Camera()
{
	Owner()->GetWindow()->GetCameras().Remove(this);
}

void gE::Camera::GetGLCamera(GL::Camera& cam) const
{
	cam.ClipPlanes = GetClipPlanes();
	cam.View[0] = View;
	cam.Projection = Projection;
	cam.Position = Owner()->GetTransform().GlobalTransform();
}

void gE::PerspectiveCamera::UpdateProjection()
{
	Projection = glm::perspectiveFov(_fov, (float) GetSize().x, (float) GetSize().y, GetClipPlanes().x, GetClipPlanes().y);
}

gE::PerspectiveCamera::PerspectiveCamera(gE::Entity* e, const gE::PerspectiveCameraSettings& s)
	: Camera2D(e, s)
{
	SetFOV(s.FOV);
	Owner()->GetWindow()->GetCameras().Register(this);
}

void gE::OrthographicCamera::UpdateProjection()
{
	Projection = glm::ortho(_orthographicScale.x, _orthographicScale.y, _orthographicScale.z, _orthographicScale.w, GetClipPlanes().x, GetClipPlanes().y);
}

gE::OrthographicCamera::OrthographicCamera(gE::Entity* e, const gE::OrthographicCameraSettings& s) :
	Camera2D(e, s), _orthographicScale(s.Scale)
{
	Owner()->GetWindow()->GetCameras().Register(this);
}

template<class TEX_T, class CAM_T>
void gE::Camera::CreateAttachments(CAM_T& cam, const gE::AttachmentSettings& settings)
{
	if(settings.Depth)
	{
		cam.DepthTexture = Reference<GL::Texture>(new TEX_T(cam.GET_WINDOW(), {settings.Depth, cam.GetSize()}));
		if constexpr (!std::is_same_v<TEX_T, GL::Texture3D>) cam.FrameBuffer.SetDepthAttachment(cam.DepthTexture);
	}

	for(u8 i = 0; i < FRAMEBUFFER_MAX_COLOR_ATTACHMENTS; i++)
	{
		if(!settings.Attachments[i]) continue;
		cam.Attachments[i] = new TEX_T(cam.GET_WINDOW(), { settings.Attachments[i], cam.GetSize() });
		if constexpr (!std::is_same_v<TEX_T, GL::Texture3D>) cam.FrameBuffer.SetAttachment(i, cam.Attachments[i]);
	}
}

gE::Camera2D::Camera2D(gE::Entity* parent, const gE::CameraSettings2D& settings) :
	Camera(parent, settings), _size(settings.Size)
{
	CreateAttachments<GL::Texture2D>(*this, settings.RenderAttachments);
}

gE::Camera3D::Camera3D(gE::Entity* parent, const gE::CameraSettings3D& settings) :
	Camera(parent, settings), _size(settings.Size)
{
	CreateAttachments<GL::Texture3D>(*this, settings.RenderAttachments);
}

void gE::Camera3D::UpdateProjection()
{
	glm::vec2 halfSize = (glm::vec2) GetSize().xz() / 2.f;
	Projection = glm::ortho(-halfSize.x, -halfSize.y, halfSize.x, halfSize.y, GetClipPlanes().x, GetClipPlanes().y);
}

gE::CubemapCamera::CubemapCamera(gE::Entity* parent, const gE::CameraSettings1D& settings) :
	Camera(parent, settings), _size(settings.Size)
{
	CreateAttachments<GL::TextureCubemap>(*this, settings.RenderAttachments);
}

void gE::CubemapCamera::UpdateProjection()
{
	Projection = glm::perspectiveFov(degree_cast<AngleType::Radian>(90.f), (float) GetSize(), (float) GetSize(), GetClipPlanes().x, GetClipPlanes().y);
}
