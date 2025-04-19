//
// Created by scion on 4/17/2025.
//

#include "SplineRenderer.h"

#include <Window.h>

namespace gE
{
    SplineRenderer::SplineRenderer(Entity* owner, const Reference<Mesh>& mesh, const Spline<Dimension::D3D>& spline) : MeshRenderer(owner, mesh),
        _spline(&spline)
    {

    }

    void SplineRenderer::OnRender(float delta, Camera* camera)
    {
        if(_spline->GetSize() <= 1) return;

        if(_enableDebugView && _spline->GetLength() != _oldLength)
        {
            _oldLength = _spline->GetLength();

            const size_t segments = _spline->GetSegmentCount() * SPLINE_LENGTH_ITERATIONS;

            GPU::VAO format = SplineDebugVAOFormat;
            format.Materials[0].Count = segments;
            format.Buffers->Data = Array<std::byte>(sizeof(glm::vec3) * segments * 2);

            glm::vec3 oldPosition = _spline->Evaluate(DistanceMode::Relative, 0.f).Position;
            for(size_t i = 0; i < segments; i++)
            {
                const size_t index = i * 2 * sizeof(glm::vec3);
                const glm::vec3 position = _spline->Evaluate(DistanceMode::Relative, (float)(i + 1) / segments).Position;

                (glm::vec3&) format.Buffers[0].Data[index] = oldPosition;
                (glm::vec3&) format.Buffers[0].Data[index + sizeof(glm::vec3)] = position;

                oldPosition = position;
            }

            _debugVAO = ptr_create<API::VAO>(&GetWindow(), std::move(format));
            _oldLength = _spline->GetLength();
        }
        if(!_enableDebugView && _debugVAO) _debugVAO.Free();

        if(GetWindow().RenderState.RenderMode != RenderMode::Fragment) return;
        if(!_enableDebugView || !_debugVAO) return;

        const RendererManager& manager = GetWindow().GetRenderers();
        const DefaultPipeline::Buffers& buffers = GetWindow().GetPipelineBuffers();

        buffers.GetScene().GetData()[0].Objects[0] = GPU::ObjectInfo{ GetOwner().GetTransform().Model() };
        buffers.GetScene().UpdateData<GPU::ObjectInfo>(1, offsetof(GPU::Scene, Objects));

        glDepthFunc(GL_ALWAYS);
        manager.GetWireframeShader().Bind();
        _debugVAO->Draw(0);
    }

    REFLECTABLE_ONGUI_IMPL(SplineRenderer, );
    REFLECTABLE_FACTORY_NO_IMPL(SplineRenderer);
}
