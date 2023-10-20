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
		if((bool) _depthFunc)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GetWindow()->GetRenderState() ==  RenderState::PreZ ? (GLenum) _depthFunc : GL_EQUAL);
		}
		else glDisable(GL_DEPTH_TEST);

		_shader->Bind();
	}

}