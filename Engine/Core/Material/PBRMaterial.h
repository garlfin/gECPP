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

namespace gE
{
	struct PBRMaterialSettings
	{
		Reference<API::Texture2D> Albedo;
		Reference<API::Texture2D> ARMD;
		Reference<API::Texture2D> Normal;
	};

	struct PBRMaterialData
	{
		vec2 Scale = vec2(1.f);
		vec2 Offset = vec2(0.f);
		float ParallaxDepth = 0.5f;
		float NormalStrength = 1.f;

		vec2 _pad;

#ifdef GE_ENABLE_IMGUI
		void OnEditorGUI(u8 depth);
#endif
	};

	class PBRMaterial : public Material
	{
		REFLECTABLE_PROTO("PBRM", PBRMaterial, Material, &Material::SType);

	public:
		PBRMaterial(Window* w, const Reference<Shader>& s, const PBRMaterialSettings& settings);

		void Bind() const override;

	private:
		gE::ReferenceUniform<API::Texture2D> _albedo;
		gE::ReferenceUniform<API::Texture2D> _armd;
		gE::ReferenceUniform<API::Texture2D> _normal;
		DynamicUniform _brdfLUT;
		PBRMaterialData _data;
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
		GET_CONST(const API::Buffer<PBRMaterialData>&, Buffer, _buffer);

	private:
		API::Buffer<PBRMaterialData> _buffer;
		API::Texture2D _brdfLUT;
	};
}