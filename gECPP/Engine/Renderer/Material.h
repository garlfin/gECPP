//
// Created by scion on 9/11/2023.
//

#pragma once
#include "GL/Shader/Shader.h"

namespace gE
{
	enum class DepthFunction : GLenum
	{
		Disable,
		Less = GL_LESS,
		LessEqual = GL_LEQUAL,
		Greater = GL_GREATER,
		GreaterEqual = GL_GEQUAL
	};

	enum class BlendMode : GLenum
	{
		Disable,
		Dither,
		Blend
	};

	struct Material
	{
	 public:
		Material(Window* window, GL::Shader* shader, DepthFunction depthFunc = DepthFunction::Less);

		virtual void Use()
		{
			if((bool) _depthFunc)
			{
				glEnable(GL_DEPTH_TEST);
				glDepthFunc((GLenum) _depthFunc);
			}
			else glDisable(GL_DEPTH_TEST);

			_shader->Bind();
		}

	 private:
		GL::Shader* const _shader;
		Window* const _window;
		const DepthFunction _depthFunc;
	};
}

gE::Material::Material(Window* window, GL::Shader* shader, DepthFunction depthFunc) :
	_window(window), _shader(shader), _depthFunc(depthFunc)
{

}