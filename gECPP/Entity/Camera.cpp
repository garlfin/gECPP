//
// Created by scion on 9/5/2023.
//

#include "Camera.h"

gE::Camera::Camera(gE::Window* w, const GL::TextureSize& size, float fov, const gE::ClipPlanes&, gE::RenderPass rp, const Array<PostProcessPass>& pp) :
	Entity(w), _fov(fov), _renderPass(rp), _postProcessPass(pp)
{

}
