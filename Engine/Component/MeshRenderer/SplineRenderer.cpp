//
// Created by scion on 4/17/2025.
//

#include "SplineRenderer.h"

namespace gE
{
    SplineRenderer::SplineRenderer(Entity* owner, const Reference<Mesh>& mesh, const Spline<Dimension::D3D>& spline) : MeshRenderer(owner, mesh),
        _spline(&spline)
    {

    }

    void SplineRenderer::OnRender(float delta, Camera* camera)
    {
        if(_spline->GetSize() <= 1) return;

        if(_enableDebugView)
            if(_debugVAO) _debugVAO.Free();
            else
            {
                size_t vertexCount = _spline->GetSegments() * 2;

            }



    }
}
