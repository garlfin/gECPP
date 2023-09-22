//
// Created by scion on 9/5/2023.
//

#include "Camera.h"
#include "Engine/Component/Transform.h"
#include "Engine/Window.h"

gE::Camera::Camera(gE::Entity* parent, const CameraSettings& settings) : Component(parent),
	_clipPlanes(settings.ClipPlanes), _size(settings.Size),
	_renderTarget(settings.RenderTarget),
	_frameBuffer(parent->GetWindow()),
	_depthTexture(parent->GetWindow(), { settings.RenderTarget->Depth, _size })
{
	GetWindow()->GetCameras().Register(this);

	if(settings.PostProcess) _postProcessPass = *settings.PostProcess;
	_frameBuffer.SetDepthAttachment(&_depthTexture);
}

void gE::Camera::OnRender(float delta)
{
	_view = glm::inverse(GetOwner()->GetTransform().Model());
	UpdateProjection();

	Window* window = GetWindow();

	GL::Camera cam;
	GetGLCamera(cam);
	window->GetPipelineBuffers()->Camera.ReplaceData(&cam);

	_frameBuffer.Bind();
	glViewport(0, 0, _size.x, _size.y);
	_renderTarget->RenderPass(window, this);

	if(!_postProcessPass.Size()) return;
}

gE::Camera::~Camera()
{
	GetWindow()->GetCameras().Remove(this);
}

void gE::PerspectiveCamera::GetGLCamera(GL::Camera& cam) const
{
	cam.ClipPlanes = _clipPlanes;
	cam.FOV = _fov;
	cam.Projection = _projection;
	cam.View[0] = _view;
	cam.Position = glm::vec3(GetOwner()->GetTransform().Model()[3]);
}

void gE::PerspectiveCamera::UpdateProjection()
{
	_projection = glm::perspectiveFov(_fov, (float) _size.x, (float) _size.y, _clipPlanes.x, _clipPlanes.y);
}

gE::PerspectiveCamera::PerspectiveCamera(gE::Entity* e, const gE::PerspectiveCameraSettings& s)
	: Camera(e, s), _fov(s.FOV)
{
	for(u8 i = 0; _renderTarget->Attachments[i].Format; i++)
	{
		_attachments[i] = new GL::Texture2D(GetOwner()->GetWindow(), { _renderTarget->Attachments[i], _size });
		_frameBuffer.SetAttachment(i, _attachments[i]);
	}
}



