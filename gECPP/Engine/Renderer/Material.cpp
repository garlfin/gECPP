//
// Created by scion on 9/29/2023.
//

#include "Material.h"
#include "PBRMaterial.h"

#include <Engine/Window.h>

namespace gE
{
	gE::Material::Material(Window* window, const Reference<API::Shader>& shader, DepthFunction depthFunc, CullMode cullMode) :
		API::Asset(window),
		_shader(shader), _depthFunc(depthFunc), _cullMode(cullMode)
	{
	}

	void gE::Material::Bind() const
	{
		gE::RenderFlags state = GetWindow().State;

		if((bool) _depthFunc && state.EnableDepthTest)
		{
			glEnable(GL_DEPTH_TEST);

			GLenum depthFunc = (GLenum) _depthFunc;
			if(state.WriteMode == WriteMode::Color) depthFunc = GL_EQUAL;

			glDepthFunc(depthFunc);
		}
		else
			glDisable(GL_DEPTH_TEST);

		if((bool) _cullMode && state.EnableFaceCull)
		{
			glEnable(GL_CULL_FACE);
			glCullFace((GLenum) _cullMode);
		}
		else
			glDisable(GL_CULL_FACE);

		if(GLAD_GL_NV_conservative_raster)
		{
			if(state.RasterMode == RasterMode::Conservative)
				glEnable(GL_CONSERVATIVE_RASTERIZATION_NV);
			else
				glDisable(GL_CONSERVATIVE_RASTERIZATION_NV);
		}

		_shader->Bind();
	}

	PBRMaterial::PBRMaterial(Window* w, const Reference<API::Shader>& s, const PBRMaterialSettings& settings) :
		Material(w, s),
		_albedo(this, "AlbedoTex", settings.Albedo),
		_amr(this, "ARMDTex", settings.AMR),
		_normal(this, "NormalTex", settings.Normal),
		_brdfLUT(&this->GetShader(), "BRDFLutTex")
	{
	}

	void gE::PBRMaterial::Bind() const
	{
		GetWindow().GetSlotManager().Reset();

		_albedo.Set();
		_amr.Set();
		_normal.Set();
		_brdfLUT.Set(GetWindow().GetBRDFLookupTexture());

		Material::Bind();
	}
}