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
		GetWindow().GetSlotManager().Reset();
	}

	REFLECTABLE_ONGUI_IMPL(Material,
	{
		DrawField(AssetDragDropField<Shader>{ "Shader" }, _shader, depth);
		DrawField(EnumField{ "Depth Function", "", EDepthFunction }, _depthFunc, depth);
		DrawField(EnumField{ "Cull Mode", "", ECullMode }, _cullMode, depth);
		DrawField(EnumField{ "Blend Mode", "", EBlendMode }, _blendMode, depth);
	});

	REFLECTABLE_FACTORY_NO_IMPL(Material);

	PBRMaterial::PBRMaterial(Window* w, const Reference<Shader>& s, const PBRMaterialSettings& settings) :
		Material(w, s),
		_albedo(GetShader(), "AlbedoTex", settings.Albedo),
		_armd(GetShader(), "ARMDTex", settings.ARMD),
		_normal(GetShader(), "NormalTex", settings.Normal),
		_brdfLUT(GetShader(), "BRDFLutTex")
	{
	}

	void PBRMaterial::Bind() const
	{
		Material::Bind();

		const PBRMaterialBuffers& manager = GetWindow().GetPBRMaterialManager();

		_albedo.Use();
		_armd.Use();
		_normal.Use();
		_brdfLUT.Use(manager.GetBRDFLUT());

		manager.GetBuffer().UpdateDataDirect(std::span(&_data, &_data + 1));
	}

#ifdef GE_ENABLE_IMGUI
	void PBRMaterialData::OnEditorGUI(u8 depth)
	{
		DrawField(ScalarField<float>{ "Scale" }, Scale, depth);
		DrawField(ScalarField<float>{ "Offset" }, Offset, depth);
		DrawField(ScalarField{ "Parllax Depth", "", 0.f, 1.f, FLT_EPSILON, ScalarViewMode::Slider }, ParallaxDepth, depth);
		DrawField(ScalarField{ "Normal Map Strength", "", 0.f, 1.f, FLT_EPSILON, ScalarViewMode::Slider }, NormalStrength, depth);
	}
#endif

	REFLECTABLE_ONGUI_IMPL(PBRMaterial,
	{
		DrawField(AssetDragDropField<API::Texture2D>{ "Albedo" }, *_albedo, depth);
		DrawField(AssetDragDropField<API::Texture2D>{ "ARMD", "AO, Roughness, Metallic, _" }, *_armd, depth);
		DrawField(AssetDragDropField<API::Texture2D>{ "Normal" }, *_normal, depth);
		DrawField(Field{ "Settings" }, _data, depth);
	});
	REFLECTABLE_FACTORY_NO_IMPL(PBRMaterial);

	PBRMaterialBuffers::PBRMaterialBuffers(Window* window) :
		_buffer(window, 1, nullptr, GPU::BufferUsageHint::Dynamic | GPU::BufferUsageHint::Write, false),
		_brdfLUT(window, BRDFLUTFormat)
	{
		_buffer.Bind(API::BufferBaseTarget::Uniform, 4);

		const API::ComputeShader brdfShader(window, GPU::ComputeShader("Resource/Shader/Compute/brdf.comp"));
		const uvec2 brdfGroupSize = DIV_CEIL_T(_brdfLUT.GetSize(), BRDF_GROUP_SIZE, uvec2);

		brdfShader.Bind();
		_brdfLUT.Bind(0, GL_WRITE_ONLY);
		brdfShader.Dispatch(brdfGroupSize);
	}

	DynamicUniform::DynamicUniform(const Shader& shader, u32 location) :
		_shader(&shader), _location(location)
	{ }

	DynamicUniform::DynamicUniform(const Shader& shader, const std::string& name) :
		_shader(&shader), _location(shader.GetReferenceShader().GetUniformLocation(name))
	{ }

	template<>
	void DynamicUniform::Use(const API::Texture& t) const
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
#ifndef GE_ENABLE_EDITOR
		_shader.Free();
#endif
	}

	REFLECTABLE_ONGUI_IMPL(ForwardShader,
		_shader.OnEditorGUI(depth);
	);

	void ForwardShader::Reload(Window* window)
	{
		 _shader.Reload(window);
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
	REFLECTABLE_ONGUI_IMPL(Shader, {})

	void Shader::Bind() const
	{
		if(const API::Shader& shader = GetShader();
			shader.GetIsCompiled()) shader.Bind();
		else
			GetWindow().GetDefaultMaterial().GetShader().Bind();
	}
}
