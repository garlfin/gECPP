//
// Created by scion on 10/25/2023.
//

#pragma once

#include <Core/Pointer.h>
#include <Core/Material/Material.h>
#include <Graphics/Texture/Texture.h>

#include "Uniform.h"

#define BRDF_SIZE 512
#define BRDF_GROUP_SIZE 8

namespace GPU
{
	struct alignas(64) PBRMaterial
	{
		vec2 Scale;
		vec2 Offset;

		float ParallaxDepth;
		float NormalStrength;
		float AOStrength;

		bool ARMDsRGB;
		bool NormalsRGB;

		handle Albedo;
		handle ARMD;
		handle Normal;
	};
}

namespace gE
{
	struct PBRMaterialSettings
	{
		Reference<API::Texture2D> Albedo;
		Reference<API::Texture2D> ARMD;
		Reference<API::Texture2D> Normal;

		vec2 Scale = vec2(10.f);
		vec2 Offset = vec2(0.f);

		float ParallaxDepth = 0.5f;
		float NormalStrength = 1.f;
		float AOStrength = 0.f;

		bool ARMDsRGB;
		bool NormalsRGB;

	#ifdef GE_ENABLE_IMGUI
		void OnEditorGUI(u8 depth);
	#endif
	};

	class PBRMaterial : public Material
	{
		REFLECTABLE_PROTO(PBRMaterial, Material, 0, &Material::SType);

	public:
		PBRMaterial(Window* w, const Reference<Shader>& s, const PBRMaterialSettings& settings);

		void Bind() const override;
		void GetGPUMaterialData(size_t index) const override;
		void FlushMaterialData(size_t size) const override;

	private:
		PBRMaterialSettings _settings;
		DynamicUniform _brdfLUT;
	};

	GLOBAL GPU::Texture2D BRDFLUTFormat = []()
	{
		GPU::Texture2D tex;
		tex.Format = GL_RG16F;
		tex.WrapMode = GPU::WrapMode::Clamp;
		tex.Size = Size2D(BRDF_SIZE);

		return tex;
	}();

	class PBRMaterialBuffers
	{
	public:
		PBRMaterialBuffers(Window* window);

		GET_CONST(const API::Texture2D&, BRDFLUT, _brdfLUT);
		GET_CONST(const API::Buffer<GPU::PBRMaterial>&, Buffer, _buffer);

	private:
		API::Buffer<GPU::PBRMaterial> _buffer;
		API::Texture2D _brdfLUT;
	};
}