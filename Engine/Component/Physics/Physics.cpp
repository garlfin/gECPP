//
// Created by scion on 11/29/2024.
//

#include "Physics.h"

#include <Window.h>
#include <Entity/Entity.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/JobSystemThreadPool.h>

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

        _physics->SetContactListener(&_listener);

        _interface = &_physics->GetBodyInterface();
    }

    void PhysicsManager::OnFixedUpdate(float delta)
    {
        const int steps = std::clamp<int>(std::floor(delta * GE_PX_MIN_TICKRATE), 1, GE_PX_MAX_STEPS);
        _physics->Update(delta, steps, _allocator.GetPointer(), _jobSystem.GetPointer());

        ComponentManager::OnFixedUpdate(delta);
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
        GE_ASSERTM(!GetOwner().GetParent(), "Physics objects must not have a parent!");
        Transform& transform = GetOwner().GetTransform();

        vec3 position;
        quat rotation;

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

        transform.SetPosition(position, TransformFlags::RenderInvalidated);
        if(UseRotation) transform.SetRotation(rotation, TransformFlags::RenderInvalidated);
    }

    void CollisionListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
    {
        if(inBody1.IsSensor())
            if(RigidBody* object = (RigidBody*) inBody1.GetUserData(); object->_isTrigger)
            {
                object->_collisionsLock.lock();
                object->_collisions.emplace_back(CollisionState::Entering, (RigidBody*) inBody2.GetUserData());
                object->_collisionsLock.unlock();
            }

        if(inBody2.IsSensor())
            if(RigidBody* object = (RigidBody*) inBody2.GetUserData(); object->_isTrigger)
            {
                object->_collisionsLock.lock();
                object->_collisions.emplace_back(CollisionState::Entering, (RigidBody*) inBody1.GetUserData());
                object->_collisionsLock.unlock();
            }
    }
}
