//
// Created by scion on 10/30/2024.
//

#include "Physics.h"

#include "Jolt/RegisterTypes.h"

namespace gE
{
    Physics::Physics(Window* window)
    {
        px::RegisterDefaultAllocator();

        px::Factory::sInstance = (px::Factory*) _factory;
        px::RegisterTypes();

        _allocator = move(ptr_create<px::TempAllocatorImpl>(GE_PX_ALLOCATION));

        _physics = move(ptr_create<px::PhysicsSystem>());
        _physics->Init
        (
            GE_PX_MAX_BODIES,
            0,
            GE_PX_MAX_BODY_PAIRS,
            GE_PX_MAX_CONSTRAINTS,
            _broadPhase,
            _broadFilter,
            _filter
        );
    }

    Physics::~Physics()
    {
        px::UnregisterTypes();
        px::Factory::sInstance = nullptr;
    }
}
