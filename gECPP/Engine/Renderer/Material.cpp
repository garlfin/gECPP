//
// Created by scion on 9/29/2023.
//

#include "Material.h"
#include "PBRMaterial.h"

#include <Engine/Window.h>

namespace gE
{
	gE::Material::Material(Window* window, const Reference<GL::Shader>& shader, DepthFunction depthFunc, CullMode cullMode) :
		GL::Asset(window), _shader(shader), _depthFunc(depthFunc), _cullMode(cullMode)
	{

	}

	void gE::Material::Bind() const
	{
		if((bool) _depthFunc)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GetWindow()->GetRenderStage() == RenderStage::PreZ ? (GLenum) _depthFunc : GL_EQUAL);
		}
		else glDisable(GL_DEPTH_TEST);

		if((bool) _cullMode)
		{
			glEnable(GL_CULL_FACE);
			glCullFace((GLenum) _cullMode);
		} else glDisable(GL_CULL_FACE);

		_shader->Bind();
	}

	PBRMaterial::PBRMaterial(Window* w, const Reference<GL::Shader>& s, const PBRMaterialSettings& settings) :
		Material(w, s),
		_albedo(*this, "Albedo", settings.Albedo),
		_amr(*this, 0u, settings.AMR),
		_normal(*this, 0u, settings.Normal)
	{

	}

	void gE::PBRMaterial::Bind() const
	{
		_albedo.Set();
	}
}