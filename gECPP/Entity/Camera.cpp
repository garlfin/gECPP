//
// Created by scion on 9/5/2023.
//

#include "Camera.h"
#include <Components/Transform.h>

gE::Camera::Camera(gE::Entity* parent, const CameraSettings& settings) :
	Component(parent), _renderPass(settings.RenderPass), _clipPlanes(settings.ClipPlanes),
	_postProcessPass(), _size(settings.Size)
{
	if(settings.PostProcess) _postProcessPass = *settings.PostProcess;
	settings.Manager->Register(this);
}

void gE::Camera::OnRender(float delta)
{
	_view = GetOwner()->GetTransform().Model().Inverse();
	UpdateProjection();

	_renderPass(GetWindow(), this);

	if(!_postProcessPass.Size()) return;
}

gE::PerspectiveCamera::PerspectiveCamera(gE::Entity* parent, const CameraSettings& settings, float fov)
	: Camera(parent, settings), _fov(fov)
{

}

GL::Camera gE::PerspectiveCamera::GetGLCamera() const
{
	GL::Camera cam;
	cam.ClipPlanes = _clipPlanes;
	cam.FOV = _fov;
	cam.Projection = _projection;
	cam.View[0] = GetOwner()->GetTransform().Model();

	return cam;
}

void gE::PerspectiveCamera::UpdateProjection()
{
	_projection = gl::mat4::Perspective(_fov, GetAspect(), _clipPlanes.x, _clipPlanes.y);
}


