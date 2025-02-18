//
// Created by scion on 10/30/2024.
//

#pragma once

#include "Physics.h"

namespace gE
{
    inline bool CollisionFilter::ShouldCollideStatic(px::ObjectLayer a, px::ObjectLayer b)
    {
        return !(a & b & (u16) LayerMask::Static) && (a & b);
    }

    inline bool CollisionFilter::ShouldCollide(px::ObjectLayer a, px::ObjectLayer b) const
    {
        return ShouldCollideStatic(a, b);
    }

    inline bool BroadPhaseFilter::ShouldCollideStatic(px::ObjectLayer a, px::BroadPhaseLayer b)
    {
        return !(a & b.GetValue() & (u8) LayerMask::Static);
    }

    inline bool BroadPhaseFilter::ShouldCollide(px::ObjectLayer a, px::BroadPhaseLayer b) const
    {
        return ShouldCollideStatic(a, b);
    }

    inline bool UCollisionFilter::ShouldCollide(JPH::ObjectLayer inLayer) const
    {
        return CollisionFilter::ShouldCollideStatic(_layer, inLayer);
    }

    inline bool UBroadPhaseFilter::ShouldCollide(JPH::BroadPhaseLayer inLayer) const
    {
        return BroadPhaseFilter::ShouldCollideStatic(_layer, inLayer);
    }

    inline BroadPhase::BroadPhase()
    {
        _layers[(u8) LayerMask::Dynamic] = px::BroadPhaseLayer((u8) LayerMask::Dynamic);
        _layers[(u8) LayerMask::Static] = px::BroadPhaseLayer((u8) LayerMask::Static);
    }

    inline px::uint BroadPhase::GetNumBroadPhaseLayers() const
    {
        return sizeof(_layers) / sizeof(px::BroadPhaseLayer);
    }

    inline px::BroadPhaseLayer BroadPhase::GetBroadPhaseLayer(JPH::ObjectLayer layer) const
    {
        return _layers[layer & (u8) LayerMask::Static];
    }
}
