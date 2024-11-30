//
// Created by scion on 11/29/2024.
//

#include "Physics.h"

#include <Jolt/RegisterTypes.h>
#include <Engine/Entity/Entity.h>

#include "Engine/Window/Window.h"

namespace gE
{
    PhysicsManager::PhysicsManager(Window* window) : ComponentManager(window)
    {
        px::RegisterDefaultAllocator();

        _factory = ptr_create<px::Factory>();

        px::Factory::sInstance = (px::Factory*) _factory;
        px::RegisterTypes();

        _allocator = ptr_create<px::TempAllocatorImpl>(GE_PX_ALLOCATION);
        _jobSystem = ptr_create<px::JobSystemThreadPool>(px::cMaxPhysicsJobs, px::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

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

        _interface = &_physics->GetBodyInterface();
    }

    void PhysicsManager::OnFixedUpdate(float delta)
    {
        const int steps = std::clamp<int>(floor(delta * GE_PX_MIN_TICKRATE), 1, GE_PX_MAX_STEPS);
        _physics->Update(delta, steps, _allocator.GetPointer(), _jobSystem.GetPointer());

        for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
            (**i)->OnFixedUpdate(delta);
    }

    void PhysicsManager::OnEarlyFixedUpdate(float delta)
    {
        OnInit();

        for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
            ((PhysicsComponent*) &***i)->OnEarlyFixedUpdate(delta);
    }

    PhysicsManager::~PhysicsManager()
    {
        px::UnregisterTypes();
        px::Factory::sInstance = nullptr;
    }

    void PhysicsComponent::OnInit()
    {
        const Transform& transform = GetOwner().GetTransform();

        PreviousPosition = Position = transform->Position;
        PreviousRotation = Rotation = transform->Rotation;
    }

    void PhysicsComponent::OnUpdate(float)
    {
        GE_ASSERT(!GetOwner().GetParent(), "Physics objects must not have a parent!");

        glm::vec3 position;
        glm::quat rotation;

        if(_interpolationMode == PhysicsInterpolationMode::None)
        {
            position = Position;
            rotation = Rotation;
        }
        else
        {
            float lerpFactor = GetWindow().GetPhysicsTick().GetLerpFactor();
            if(_interpolationMode == PhysicsInterpolationMode::Extrapolate) lerpFactor += 1.f;

            position = mix(PreviousPosition, Position, lerpFactor);
            rotation = slerp(PreviousRotation, Rotation, lerpFactor);
        }

        Transform& transform = GetOwner().GetTransform();
        transform.SetPosition(position, TransformFlags::RenderInvalidated);
        transform.SetRotation(rotation, TransformFlags::RenderInvalidated);
    }
}
