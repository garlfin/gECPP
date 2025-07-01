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
	Material::Material(Window* window, const Reference<Shader>& shader) :
		_window(window), _shader(shader)
	{
	}

	void Material::Bind() const
	{
		_shader->Bind();
		GetWindow().GetSlotManager().Reset();
	}

	REFLECTABLE_ONGUI_IMPL(Material,
	{
		//DrawField(AssetDragDropField<Shader>{ "Shader" }, _shader, depth);
	})
	REFLECTABLE_FACTORY_NO_IMPL(Material);

	PBRMaterial::PBRMaterial(Window* w, const Reference<Shader>& s, const PBRMaterialSettings& settings) :
		Material(w, s),
		_settings(settings),
		_brdfLUT(GetShader(), "BRDFLutTex")
	{
	}

	void PBRMaterial::Bind() const
	{
		Material::Bind();
		_brdfLUT.Use(GetWindow().GetPBRMaterialManager().GetBRDFLUT());
	}

	void PBRMaterial::GetGPUMaterialData(size_t index) const
	{
		GPU::PBRMaterial& data = GetWindow().GetPBRMaterialManager().GetBuffer().GetData()[index];

		data.Albedo = _settings.Albedo->GetHandle();
		data.ARMD = _settings.ARMD->GetHandle();
		data.Normal = _settings.Normal->GetHandle();
		data.Scale = _settings.Scale;
		data.Offset = _settings.Offset;

		data.ParallaxDepth = _settings.ParallaxDepth;
		data.NormalStrength = _settings.NormalStrength;
		data.AOStrength = _settings.AOStrength;

		data.NormalsRGB = _settings.NormalsRGB;
		data.ARMDsRGB = _settings.ARMDsRGB;
	}

	void PBRMaterial::FlushMaterialData(size_t size) const
	{
		GetWindow().GetPBRMaterialManager().GetBuffer().UpdateData(size);
	}

#ifdef GE_ENABLE_IMGUI
	void PBRMaterialSettings::OnEditorGUI(u8 depth)
	{
		/*DrawField(ScalarField<float>{ "Scale" }, Scale, depth);
		DrawField(ScalarField<float>{ "Offset" }, Offset, depth);
		DrawField(ScalarField{ "Parllax Depth", "", 0.f, 1.f, FLT_EPSILON, ScalarViewMode::Slider }, ParallaxDepth, depth);
		DrawField(ScalarField{ "Normal Map Strength", "", 0.f, 1.f, FLT_EPSILON, ScalarViewMode::Slider }, NormalStrength, depth);
		DrawField(ScalarField{ "AO Strength", "", 0.f, 5.f, FLT_EPSILON, ScalarViewMode::Slider }, AOStrength, depth);

		DrawField(AssetDragDropField<API::Texture2D>{ "Albedo" }, Albedo, depth);
		DrawField(Field{"Normal sRGB Color Space"}, NormalsRGB, depth);
		DrawField(AssetDragDropField<API::Texture2D>{ "Normal" }, Normal, depth);
		DrawField(Field{"ARMD sRGB Color Space"}, ARMDsRGB, depth);
		DrawField(AssetDragDropField<API::Texture2D>{ "ARMD" }, ARMD, depth);*/
	}
#endif

	REFLECTABLE_ONGUI_IMPL(PBRMaterial,
	{
		// DrawField(Field{ "Settings" }, _settings, depth);
	});
	REFLECTABLE_FACTORY_NO_IMPL(PBRMaterial);

	PBRMaterialBuffers::PBRMaterialBuffers(Window* window) :
		_buffer(window, API_MAX_INSTANCE, nullptr, GPU::BufferUsageHint::Dynamic | GPU::BufferUsageHint::Write, true),
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
		_gBufferShader.Free();
	}

	bool DeferredShader::IsFree() const
	{
		return _forwardShader.IsFree() && _gBufferShader.IsFree();
	}

#ifdef DEBUG
	bool DeferredShader::VerifyUniforms(const std::string& name) const
	{
		const u32 a = _forwardShader.GetUniformLocation(name);
		const u32 b = _gBufferShader.GetUniformLocation(name);
		return a == b;
	}
#endif

	REFLECTABLE_FACTORY_NO_IMPL(Shader);
	REFLECTABLE_ONGUI_IMPL(Shader,
		/*DrawField(EnumField{ "Depth Function", "", EDepthFunction }, _depthFunc, depth);
		DrawField(EnumField{ "Cull Mode", "", ECullMode }, _cullMode, depth);
		DrawField(EnumField{ "Blend Mode", "", EBlendMode }, _blendMode, depth);*/
	)

	void Shader::Bind() const
	{
		const RenderFlags state = GetWindow().RenderState;

		if(const API::Shader& shader = GetShader();
			shader.GetIsCompiled()) shader.Bind();
		else
			GetWindow().GetDefaultMaterial().GetShader().Bind();

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
	}
}
