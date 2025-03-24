//
// Created by scion on 10/6/2023.
//

#pragma once

#include <Component/MeshRenderer/MeshRenderer.h>
#include <Entity/Entity.h>

namespace gE
{
    class StaticMeshEntity : public Entity
    {
        REFLECTABLE_PROTO("gE::VoxelDemo::StaticMeshEntity", "STME", StaticMeshEntity, Entity);

    public:
        StaticMeshEntity(Window* window, const Reference<Mesh>& mesh) :
            Entity(window),
            _renderer(this, mesh) {};

        GET(gE::MeshRenderer&, Renderer, _renderer);

    private:
        MeshRenderer _renderer;
    };

    REFLECTABLE_FACTORY_NO_IMPL(StaticMeshEntity, inline);
    inline REFLECTABLE_ONGUI_IMPL(StaticMeshEntity,
    {
        DrawField(Field{ "Renderer"sv }, _renderer, depth);
    });
}
