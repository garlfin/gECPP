//
// Created by scion on 9/5/2023.
//

#include "Camera.h"
#include "Engine/Component/Transform.h"
#include "Engine/Window.h"

gE::Camera::Camera(gE::Entity* parent, const SizelessCameraSettings& settings) : Component(parent),
																				 ClipPlanes(settings.ClipPlanes),
																				 RenderTarget(settings.RenderTarget),
																				 FrameBuffer(parent->GetWindow())
{
	GetWindow()->GetCameras().Register(this);
	if(settings.PostProcess) PostProcessPasses = *settings.PostProcess;
}

void gE::Camera::OnRender(float delta)
{
	View = glm::inverse(GetOwner()->GetTransform().Model());
	UpdateProjection();

	Window* window = GetWindow();

	GL::Camera cam;
	GetGLCamera(cam);
	window->GetPipelineBuffers()->Camera.ReplaceData(&cam);

	FrameBuffer.Bind();
	RenderTarget->RenderPass(window, this);

	if(!PostProcessPasses.Size()) return;
}

gE::Camera::~Camera()
{
	GetWindow()->GetCameras().Remove(this);
}

void gE::PerspectiveCamera::GetGLCamera(GL::Camera& cam) const
{
	cam.ClipPlanes = GetClipPlanes();
	cam.FOV = _fov;
	cam.Projection = Projection;
	cam.View[0] = View;
	cam.Position = glm::vec3(GetOwner()->GetTransform().Model()[3]);
}

void gE::PerspectiveCamera::UpdateProjection()
{
	Projection = glm::perspectiveFov(_fov, (float) GetSize().x, (float) GetSize().y, GetClipPlanes().x, GetClipPlanes().y);
}

gE::PerspectiveCamera::PerspectiveCamera(gE::Entity* e, const gE::PerspectiveCameraSettings& s)
	: Camera2D(e, s), _fov(s.FOV)
{
}

void gE::OrthographicCamera::UpdateProjection()
{
	Projection = glm::ortho(_orthographicScale.x, _orthographicScale.y, _orthographicScale.z, _orthographicScale.w, GetClipPlanes().x, GetClipPlanes().y);
}

gE::OrthographicCamera::OrthographicCamera(gE::Entity* e, const gE::OrthographicCameraSettings& s) :
	Camera2D(e, s), _orthographicScale(s.Scale)
{}

gE::Camera2D::Camera2D(gE::Entity *parent, const gE::CameraSettings2D& settings) :
	Camera(parent, settings), _size(settings.Size)
{
	DepthTexture = new GL::Texture2D(parent->GetWindow(), { settings.RenderTarget->Depth, _size });
	FrameBuffer.SetDepthAttachment(DepthTexture);

	for(u8 i = 0; GetRenderTarget()->Attachments[i].Format; i++)
	{
		Attachments[i] = new GL::Texture2D(GetOwner()->GetWindow(), { GetRenderTarget()->Attachments[i], _size });
		FrameBuffer.SetAttachment(i, Attachments[i]);
	}
}

