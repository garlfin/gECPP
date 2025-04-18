//
// Created by scion on 4/17/2025.
//

#pragma once

#include <Core/Pointer.h>
#include <Core/Math/Spline.h>
#include <Graphics/Buffer/VAO.h>

#include "MeshRenderer.h"

namespace gE
{
    GLOBAL GPU::VAO SplineRendererVAOFormat = []()
    {
        GPU::VAO result = DEFAULT;

        result.PrimitiveType = GPU::PrimitiveType::Line;
        result.AddBuffer(DEFAULT);
        result.AddField(GPU::VertexField("POS", GPU::ElementType::Float, false, 0, 0, 3, 0));
        result.AddMaterial(DEFAULT);

        return result;
    }();

    class SplineRenderer : public MeshRenderer
    {
        REFLECTABLE_PROTO("SPLR", SplineRenderer, MeshRenderer);

    public:
        SplineRenderer(Entity* owner, const Reference<Mesh>& mesh, const Spline<Dimension::D3D>& spline);

        GET_CONST(const Spline<Dimension::D3D>&, Spline, *_spline);
        SET(const Spline<Dimension::D3D>&, Spline, _spline, &);

        void OnRender(float delta, Camera* camera) override;

    private:
        const Spline<Dimension::D3D>* _spline = nullptr;

    #ifdef GE_ENABLE_EDITOR
            bool _enableDebugView = false;
            Pointer<API::VAO> _debugVAO;
    #endif
    };
}
