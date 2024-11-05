//
// Created by scion on 10/30/2024.
//

#pragma once

#include "Physics.h"
#include "Engine/Component/Layer.h"

namespace gE
{
    inline bool CollisionFilter::ShouldCollide(px::ObjectLayer a, px::ObjectLayer b) const
    {
        return a & b;
    }

    inline bool BroadPhaseFilter::ShouldCollide(px::ObjectLayer a, px::BroadPhaseLayer b) const
    {
        return !(a & b.GetValue() & (u8) LayerMask::Static);
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
