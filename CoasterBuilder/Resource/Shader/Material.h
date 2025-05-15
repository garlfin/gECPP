//
// Created by scion on 4/11/2025.
//

#pragma once

#include <Core/Material/Material.h>
#include <Core/Material/Uniform.h>
#include <Graphics/API/GL/Texture/Texture.h>

namespace Coaster
{
    class Material : public gE::Material
    {
    public:
        Material(gE::Window* window, const Reference<Shader>& shader, const Reference<API::Texture2D>& albedo) : gE::Material(window, shader),
            _albedo(shader, "AlbedoTex", albedo),
            _brdfLUT(GetShader(), "BRDFLutTex")
        {};

        void Bind() const override
        {
            gE::Material::Bind();

            const PBRMaterialBuffers& manager = GetWindow().GetPBRMaterialManager();

            _albedo.Use();
            _brdfLUT.Use(manager.GetBRDFLUT());
        }

    private:
        gE::ReferenceUniform<API::Texture2D> _albedo;
        DynamicUniform _brdfLUT;
    };
}
