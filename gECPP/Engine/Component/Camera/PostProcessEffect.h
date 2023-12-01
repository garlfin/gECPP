//
// Created by scion on 11/28/2023.
//

#pragma once

#include "Prototype.h"
#include "RenderTarget.h"
#include <GL/Texture/Texture.h>

namespace gE
{
	template<class T>
	class PostProcessEffect
	{
	 public:
		explicit PostProcessEffect(T& target) : _target(target) {};

		virtual void RenderPass(T&, GL::Texture&, GL::Texture&) = 0;

		GET(T&, Target, _target);
		GET(T::CAMERA_TYPE&, Camera, _target.GetCamera());

	 private:
		T& _target;
	};
}