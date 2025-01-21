
//
// Created by scion on 11/29/2024.
//

#pragma once

#include <Physics/Physics.h>
#include <Engine/Component/Component.h>
#include <Engine/Utility/AssetManager.h>
#include <Jolt/Core/JobSystemThreadPool.h>

namespace gE
{
    enum class PhysicsInterpolationMode : u8
    {
        None,
        Interpolate,
        Extrapolate
    };

    class PhysicsComponent : public Component
    {
    public:
        using Component::Component;

        void OnInit() override;
        void OnUpdate(float d) final;

        virtual void ForceUpdateTransforms() = 0;
        virtual void OnEarlyFixedUpdate(float d) = 0;

        GET_SET_VALUE(PhysicsInterpolationMode, InterpolationMode, _interpolationMode);

    protected:
        void ResetTransformFlag();

        glm::vec3 Position, PreviousPosition;
        glm::quat Rotation, PreviousRotation;

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
        CollisionFilter _filter;
        BroadPhaseFilter _broadFilter;
        BroadPhase _broadPhase;

        Pointer<px::Factory> _factory;
        Pointer<px::TempAllocatorImpl> _allocator;
        Pointer<px::PhysicsSystem> _physics;
        Pointer<px::JobSystemThreadPool> _jobSystem;
        px::BodyInterface* _interface;
    };
}
