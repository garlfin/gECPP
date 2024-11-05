//
// Created by scion on 11/3/2024.
//

#include "Collider.h"

namespace gE
{
    SphereCollider::SphereCollider(Entity* owner, const SphereColliderSettings& s) :
        RigidBody(owner, s, *_shape),
        _shape(s.Radius)
    {
        _shape->SetDensity(s.Mass / _shape->GetVolume());

        FinalizeConstruction();
    }

    BoxCollider::BoxCollider(Entity* owner, const BoxColliderSettings& s) :
        RigidBody(owner, s, *_shape),
        _shape(ToPX(s.Extents))
    {
        _shape->SetDensity(s.Mass / _shape->GetVolume());

        FinalizeConstruction();
    }
}
