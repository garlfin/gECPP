//
// Created by scion on 10/6/2023.
//

#pragma once

#include <Component/MeshRenderer/MeshRenderer.h>
#include <Entity/Entity.h>

namespace gE::VoxelDemo
{
    class StaticMeshEntity : public Entity
    {
        REFLECTABLE_PROTO(StaticMeshEntity, Entity, "gE::VoxelDemo::StaticMeshEntity");

    public:
        StaticMeshEntity(Window* window, const Reference<Mesh>& mesh) :
            Entity(window),
            _renderer(this, mesh) {};

        GET(gE::MeshRenderer&, Renderer, _renderer);

    private:
        MeshRenderer _renderer;
    };

    inline REFLECTABLE_FACTORY_NO_IMPL(StaticMeshEntity);

    inline REFLECTABLE_ONGUI_IMPL(StaticMeshEntity,
    {
        DrawField(Field{ "Renderer"sv }, _renderer, depth);
    });
}
