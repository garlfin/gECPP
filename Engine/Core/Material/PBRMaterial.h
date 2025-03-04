//
// Created by scion on 10/25/2023.
//

#pragma once

#include <Core/Pointer.h>
#include <Core/Material/Material.h>
#include <Graphics/Texture/Texture.h>

#include "Uniform.h"

namespace gE
{
	struct PBRMaterialSettings
	{
		Reference<API::Texture2D> Albedo;
		Reference<API::Texture2D> ARMD;
		Reference<API::Texture2D> Normal;
	};

	class PBRMaterial : public Material
	{
		REFLECTABLE_PROTO(PBRMaterial, Material, "gE::PBRMaterial", &Material::Type);

	public:
		PBRMaterial(Window* w, const Reference<Shader>& s, const PBRMaterialSettings& settings);

		void Bind() const override;

	private:
		gE::ReferenceUniform<API::Texture2D> _albedo;
		gE::ReferenceUniform<API::Texture2D> _armd;
		gE::ReferenceUniform<API::Texture2D> _normal;
		DynamicUniform _brdfLUT;
	};
}