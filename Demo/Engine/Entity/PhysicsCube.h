//
// Created by scion on 11/4/2024.
//

#pragma once

#include <Component/Physics/Collider.h>
#include <Entity/Entity.h>

namespace gE
{
    inline Physics::BoxShape GetShapeSettings(const vec3& extents);

    class PhysicsCubeEntity final : public Entity
    {
        REFLECTABLE_PROTO(PhysicsCubeEntity, Entity);

    public:
        PhysicsCubeEntity(Window* window, const Reference<Mesh>& mesh, vec3 size, EntityFlags flags = DEFAULT) :
            Entity(window, nullptr, LayerMask::All, flags),
            _renderer(this, mesh),
            _rigidBody(this, RigidBodySettings(), _collider),
            _collider(this, GetShapeSettings(size))
        {
        }

        GET(MeshRenderer&, Renderer, _renderer);
        GET(RigidBody&, RigidBody, _rigidBody);
        GET(BoxCollider&, Collider, _collider);

    private:
        MeshRenderer _renderer;
        RigidBody _rigidBody;
        BoxCollider _collider;
    };

    REFLECTABLE_FACTORY_NO_IMPL(PhysicsCubeEntity, inline);
    inline REFLECTABLE_ONGUI_IMPL(PhysicsCubeEntity,
    {
        DrawField(Field{ "Mesh Renderer"sv }, _renderer, depth);
        DrawField(Field{ "Rigid Body"sv }, _rigidBody, depth);
        DrawField(Field{ "Box Collider"sv }, _collider, depth);
    });
}
