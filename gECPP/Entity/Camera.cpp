//
// Created by scion on 9/5/2023.
//

#include "Camera.h"
#include <Components/Transform.h>

gE::Camera::Camera(gE::Window* w, const GL::TextureSize& size, const gE::ClipPlanes&, gE::RenderPass rp, const Array<PostProcessPass>* pp) :
	Entity(w), _renderPass(rp)
{
	_postProcessPass = pp ? new Array<PostProcessPass>(*pp) : nullptr;
}
void gE::Camera::OnRender(float delta)
{
	Entity::OnRender(delta);
	GetModel() = GetModel().Inverse();

	UpdateProjection();

	_renderPass(this);

	if(!_postProcessPass) return;


}

gE::PerspectiveCamera::PerspectiveCamera(gE::Window* w, float fov, const GL::TextureSize& s, const gE::ClipPlanes& cp, gE::RenderPass rp, const Array<PostProcessPass>* pp)
	: Camera(w, s, cp, rp, pp), _fov(fov)
{

}

GL::Camera gE::PerspectiveCamera::GetGLCamera() const
{

	GL::Camera cam;
	cam.ClipPlanes = _clipPlanes;
	cam.FOV = _fov;
	cam.Projection = _projection;
	cam.View[0] = GetTransform().Model();

	return cam;
}

void gE::PerspectiveCamera::UpdateProjection()
{
	_projection = gl::mat4::Perspective(_fov, GetAspect(), _clipPlanes.x, _clipPlanes.y);
}


