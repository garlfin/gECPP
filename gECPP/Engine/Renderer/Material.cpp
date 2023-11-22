//
// Created by scion on 9/29/2023.
//

#include "Material.h"
#include "PBRMaterial.h"

#include <Engine/Window.h>

namespace gE
{
	gE::Material::Material(Window* window, const Reference<GL::Shader>& shader, DepthFunction depthFunc, CullMode cullMode) :
		GL::Asset(window),
		_shader(shader), _depthFunc(depthFunc), _cullMode(cullMode),
		_colorUniform(_shader.Get(), "CameraColor"), _depthUniform(_shader.Get(), "CameraDepth")
	{
	}

	void gE::Material::Bind() const
	{
		if((bool) _depthFunc)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GetWindow().Stage == RenderStage::PreZ ? (GLenum) _depthFunc : GL_EQUAL);
		}
		else
			glDisable(GL_DEPTH_TEST);

		if((bool) _cullMode)
		{
			glEnable(GL_CULL_FACE);
			glCullFace((GLenum) _cullMode);
		}
		else
			glDisable(GL_CULL_FACE);

		gE::Camera* camera = GetWindow().GetCameras().CallingCamera;
		GE_ASSERT(camera, "NO CALLING CAMERA!");

		if(GL::Texture* t = camera->GetDepthAttachmentCopy()) _depthUniform.Set(*(GL::Texture2D*) t);
		else _depthUniform.Set(*(GL::Texture2D*) camera->GetDepthAttachment());

		if(GL::Texture* t = camera->GetAttachment<0, true>()) _colorUniform.Set(*(GL::Texture2D*) t);

		_shader->Bind();
	}

	PBRMaterial::PBRMaterial(Window* w, const Reference<GL::Shader>& s, const PBRMaterialSettings& settings) :
		Material(w, s),
		_albedo(this, "AlbedoTex", settings.Albedo),
		_amr(this, "AMRTex", settings.AMR),
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