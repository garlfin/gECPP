//
// Created by scion on 10/30/2024.
//

#include "Physics.h"

#include "Jolt/RegisterTypes.h"

namespace gE
{
    PhysicsManager::PhysicsManager(Window* window)
    {
        px::RegisterDefaultAllocator();

        _factory = ptr_create<px::Factory>();

        px::Factory::sInstance = (px::Factory*) _factory;
        px::RegisterTypes();

        _allocator = ptr_create<px::TempAllocatorImpl>(GE_PX_ALLOCATION);
        _jobSystem = ptr_create<px::JobSystemThreadPool>(px::cMaxPhysicsJobs, px::cMaxPhysicsBarriers, std::thread::hardware_concurrency());

        _physics = ptr_create<px::PhysicsSystem>();
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

    void PhysicsManager::Simulate(float delta)
    {
        const int steps = ceil(delta * GE_PX_MIN_TICKRATE);
        _physics->Update(delta, steps, _allocator.Get(), _jobSystem.Get());
    }

    PhysicsManager::~PhysicsManager()
    {
        px::UnregisterTypes();
        px::Factory::sInstance = nullptr;
    }
}
