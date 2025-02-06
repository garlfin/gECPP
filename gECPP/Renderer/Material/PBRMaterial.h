//
// Created by scion on 10/25/2023.
//

#pragma once

#include <Renderer/Material/Material.h>
#include <Utility/AssetManager.h>
#include <Graphics/Texture/Texture.h>

#include "Uniform.h"

namespace gE
{
	struct PBRMaterialSettings
	{
		gE::Reference<API::Texture2D> Albedo;
		gE::Reference<API::Texture2D> AMR;
		gE::Reference<API::Texture2D> Normal;
	};

	class PBRMaterial : public Material
	{
	 public:
		PBRMaterial(Window* w, const Reference<Shader>& s, const PBRMaterialSettings& settings);

		void Bind() const override;

	 private:
		const gE::ReferenceUniform<API::Texture2D> _albedo;
		const gE::ReferenceUniform<API::Texture2D> _amr;
		const gE::ReferenceUniform<API::Texture2D> _normal;
		const DynamicUniform _brdfLUT;
	};
}