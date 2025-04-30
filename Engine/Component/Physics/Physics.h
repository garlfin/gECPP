
//
// Created by scion on 11/29/2024.
//

#pragma once

#include <Component/Component.h>
#include <Core/Pointer.h>
#include <Jolt/Jolt.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Physics/Physics.h>

namespace gE
{
    class PhysicsComponent;
    class RigidBody;

    enum class PhysicsInterpolationMode : u8
    {
        None,
        Interpolate,
        Extrapolate
    };

    enum class CollisionState : u8
    {
        Changed = 1,
        Intersecting = 1 << 1,

        NoCollision = 0,
        Entering = Changed | Intersecting,
        Persisting = Intersecting,
        Exiting = Changed,
    };

    ENUM_OPERATOR(CollisionState, |);
    ENUM_OPERATOR(CollisionState, &);
    ENUM_OPERATOR_UNARY(CollisionState, ~);

    struct Collision
    {
        CollisionState State;
        RigidBody* Object;
    };

    class PhysicsComponent : public Component
    {
    public:
        using Component::Component;

        void OnInit() override;
        void OnUpdate(float d) override;

        virtual void ForceUpdateTransforms() = 0;
        virtual void OnEarlyFixedUpdate(float d) = 0;

        GET_SET(PhysicsInterpolationMode, InterpolationMode, _interpolationMode);

    protected:
        bool UseRotation = true;

        vec3 Position, PreviousPosition;
        quat Rotation, PreviousRotation;

    private:
        PhysicsInterpolationMode _interpolationMode = PhysicsInterpolationMode::Interpolate;
    };

    class PhysicsManager final : public ComponentManager<Component>
    {
    public:
        explicit PhysicsManager(Window* window);

        void OnFixedUpdate(float delta) override;
        void OnEarlyFixedUpdate(float delta);

        GET(CollisionFilter&, Filter, _filter);
        GET(BroadPhaseFilter&, BroadFilter, _broadFilter);
        GET(px::PhysicsSystem&, System, _physics);
        GET(px::TempAllocator&, TempAllocator, _allocator);

        friend class RigidBody;

        ~PhysicsManager() override;

    private:
        CollisionFilter _filter = DEFAULT;
        BroadPhaseFilter _broadFilter = DEFAULT;
        BroadPhase _broadPhase = DEFAULT;
        CollisionListener _listener = DEFAULT;

        Pointer<px::Factory> _factory;
        Pointer<px::TempAllocatorImpl> _allocator;
        Pointer<px::PhysicsSystem> _physics;
        Pointer<px::JobSystemThreadPool> _jobSystem;
        px::BodyInterface* _interface;
    };
}
