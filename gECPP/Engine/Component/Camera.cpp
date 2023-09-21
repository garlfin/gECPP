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
	if(settings.PostProcess) _postProcessPass = *settings.PostProcess;
	GetWindow()->GetCameras().Register(this);
}

void gE::Camera::OnRender(float delta)
{
	_view = glm::inverse(GetOwner()->GetTransform().Model());
	UpdateProjection();

	Window* window = GetWindow();

	GL::Camera cam = GetGLCamera();
	window->GetPipelineBuffers()->Camera.ReplaceData(&cam);

	_renderTarget->RenderPass(window, this);

	if(!_postProcessPass.Size()) return;
}

gE::Camera::~Camera()
{
	GetWindow()->GetCameras().Remove(this);
}

GL::Camera gE::PerspectiveCamera::GetGLCamera() const
{
	GL::Camera cam;
	cam.ClipPlanes = _clipPlanes;
	cam.FOV = _fov;
	cam.Projection = _projection;
	cam.View[0] = _view;
	cam.Position = glm::vec3(GetOwner()->GetTransform().Model()[3]);

	return cam;
}

void gE::PerspectiveCamera::UpdateProjection()
{
	_projection = glm::perspectiveFov(_fov, (float) _size.x, (float) _size.y, _clipPlanes.x, _clipPlanes.y);
}

gE::PerspectiveCamera::PerspectiveCamera(gE::Entity* e, const gE::PerspectiveCameraSettings& s)
	: Camera(e, s), _fov(s.FOV)
{
	for(u8 i = 0; _renderTarget->Attachments[i].Format; i++)
		_attachments[i] = new GL::Texture2D(GetOwner()->GetWindow(), { _renderTarget->Attachments[i], _size });
}



