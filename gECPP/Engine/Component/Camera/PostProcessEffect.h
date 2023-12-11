//
// Created by scion on 11/28/2023.
//

#pragma once

#include "Prototype.h"
#include "RenderTarget.h"
#include <GL/Texture/Texture.h>

namespace gE
{
	// Currently, post process effects don't support per-camera settings.
	// I don't have time to work on this. I'll add it in a later version.

	template<class T>
	class PostProcessEffect
	{
	 public:
		explicit PostProcessEffect(T& target) : _target(target) {};

		virtual void RenderPass(T::TEX_T&, T::TEX_T&) = 0;

		GET(T&, Target, _target);

	 private:
		T& _target;
	};
}