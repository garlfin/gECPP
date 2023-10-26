//
// Created by scion on 10/25/2023.
//

#pragma once

#include <GL/Shader/Shader.h>
#include <GL/Texture/Texture.h>
#include "Engine/Renderer/Material.h"
#include "Engine/AssetManager.h"

namespace gE
{
	struct PBRMaterialSettings
	{
		gE::Reference<GL::Texture2D>& Albedo;
		gE::Reference<GL::Texture2D>& AMR;
		gE::Reference<GL::Texture2D>& Normal;
	};

	struct PBRMaterial : public Material
	{
	 public:
		PBRMaterial(Window* w, const Reference<GL::Shader>& s, PBRMaterialSettings& settings);

		void Bind() const override;

	 private:
		const gE::ReferenceUniform<GL::Texture2D> _albedo;
		const gE::ReferenceUniform<GL::Texture2D> _amr;
		const gE::ReferenceUniform<GL::Texture2D> _normal;
	};
}