//
// Created by scion on 9/5/2023.
//

#include "Camera.h"
#include "Engine/Component/Transform.h"
#include "Engine/Window.h"

gE::Camera::Camera(gE::Entity* parent, const CameraSettings& settings) :
	Component(parent), _clipPlanes(settings.ClipPlanes), _size(settings.Size)
{
	if(settings.PostProcess) _postProcessPass = *settings.PostProcess;
	GetWindow()->GetCameras().Register(this);
}

void gE::Camera::OnRender(float delta)
{
	_view = glm::inverse(GetOwner()->GetTransform().Model());
	UpdateProjection();

	Window* window = GetWindow();
	window->GetPipelineBuffers()->UpdateCamera(GetGLCamera());

	_renderPass(window, this);

	if(!_postProcessPass.Size()) return;
}

gE::Camera::~Camera()
{
	GetWindow()->GetCameras().Remove(this);
}

template<class T>
GL::Camera gE::PerspectiveCamera<T>::GetGLCamera() const
{
	GL::Camera cam;
	cam.ClipPlanes = _clipPlanes;
	cam.FOV = _fov;
	cam.Projection = _projection;
	cam.View[0] = _view;
	cam.Position = glm::vec3(GetOwner()->GetTransform().Model()[3]);

	return cam;
}

template<class T>
void gE::PerspectiveCamera<T>::UpdateProjection()
{
	_projection = glm::perspectiveFov(_fov, (float) _size.x, (float) _size.y, _clipPlanes.x, _clipPlanes.y);
}



