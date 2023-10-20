//
// Created by scion on 9/29/2023.
//

#include "Material.h"
#include <Engine/Window.h>

namespace gE
{
	gE::Material::Material(Window* window, const Handle<GL::Shader>& shader, DepthFunction depthFunc) :
		GL::Asset(window), _shader(shader), _depthFunc(depthFunc)
	{

	}

	void gE::Material::Bind() const
	{
		if(GetWindow())
		if((bool) _depthFunc)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc((GLenum) _depthFunc);
		}
		else glDisable(GL_DEPTH_TEST);

		_shader->Bind();
	}

}