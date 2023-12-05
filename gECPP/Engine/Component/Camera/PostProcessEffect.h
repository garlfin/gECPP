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
		explicit PostProcessEffect(Window* window) : _window(*window) {};

		virtual void RenderPass(T&, GL::Texture&, GL::Texture&) = 0;

		GET(Window&, Window, _window);

	 private:
		Window& _window;
	};
}