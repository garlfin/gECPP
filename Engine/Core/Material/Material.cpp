//
// Created by scion on 9/29/2023.
//

#include "Material.h"

#include <Window.h>

#include "PBRMaterial.h"
#include "Shader.h"
#include "Uniform.h"

namespace gE
{
	Material::Material(Window* window, const Reference<Shader>& shader, DepthFunction depthFunc, CullMode cullMode) :
		_shader(shader), _depthFunc(depthFunc), _cullMode(cullMode), _window(window)
	{
	}

	void Material::Bind() const
	{
		const RenderFlags state = GetWindow().RenderState;

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

		glDisable(GL_BLEND);

		_shader->Bind();
	}

	REFLECTABLE_ONGUI_IMPL(Material,
	{
		DrawField(Field{ "Shader"sv }, _shader, depth);
		DrawField(EnumField{ "Depth Function", ""sv, EDepthFunction }, _depthFunc, depth);
		DrawField(EnumField{ "Cull Mode", ""sv, ECullMode }, _cullMode, depth);
		DrawField(EnumField{ "Blend Mode", ""sv, EBlendMode }, _blendMode, depth);
	});

	REFLECTABLE_FACTORY_NO_IMPL(Material);

	PBRMaterial::PBRMaterial(Window* w, const Reference<Shader>& s, const PBRMaterialSettings& settings) :
		Material(w, s),
		_albedo(GetShader(), "AlbedoTex", settings.Albedo),
		_amr(GetShader(), "ARMDTex", settings.AMR),
		_normal(GetShader(), "NormalTex", settings.Normal),
		_brdfLUT(GetShader(), "BRDFLutTex")
	{
	}

	void PBRMaterial::Bind() const
	{
		GetWindow().GetSlotManager().Reset();

		_albedo.Set();
		_amr.Set();
		_normal.Set();
		_brdfLUT.Set(GetWindow().GetBRDFLookupTexture());

		Material::Bind();
	}

	DynamicUniform::DynamicUniform(const Shader& shader, u32 location) :
		_shader(&shader), _location(location)
	{ }

	DynamicUniform::DynamicUniform(const Shader& shader, const std::string& name) :
		_shader(&shader), _location(shader.GetReferenceShader().GetUniformLocation(name))
	{ }

	template<>
	void DynamicUniform::Set(const API::Texture& t) const
	{
		_shader->GetShader().SetUniform(_location, t, _shader->GetWindow().GetSlotManager().Increment(&t));
	}

#ifdef DEBUG
	bool Shader::VerifyUniforms(u32 index) const
	{
		return VerifyUniforms(GetReferenceShader().GetUniformName(index));
	}
#endif

	ForwardShader::ForwardShader(Window* window, const GPU::Shader& source) : Shader(window),
	    _shader(window, source)
	{
		_shader.Free();
	}

	DeferredShader::DeferredShader(Window* window, const GPU::Shader& source) : Shader(window)
	{
	}

	const API::Shader& DeferredShader::GetShader() const
	{
		switch(GetWindow().RenderState.RenderMode)
		{
		case RenderMode::Both:
			return _forwardShader;

		case RenderMode::Fragment:
			return _deferredShader;

		case RenderMode::Geometry:
			return _gBufferShader;

		default:
			GE_FAIL("UNEXPECTED RENDER MODE!");
			return *(API::Shader*) nullptr;
		}
	}

	void DeferredShader::Free()
	{
		_forwardShader.Free();
		_deferredShader.Free();
		_gBufferShader.Free();
	}

	bool DeferredShader::IsFree() const
	{
		return _forwardShader.IsFree() && _deferredShader.IsFree() && _gBufferShader.IsFree();
	}

#ifdef DEBUG
	bool DeferredShader::VerifyUniforms(const std::string& name) const
	{
		const u32 a = _forwardShader.GetUniformLocation(name);
		const u32 b = _deferredShader.GetUniformLocation(name);
		const u32 c = _gBufferShader.GetUniformLocation(name);
		return a == b && b == c;
	}
#endif

	REFLECTABLE_FACTORY_NO_IMPL(Shader);
	REFLECTABLE_ONGUI_IMPL(Shader, {});
}
