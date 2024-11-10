//
// Created by scion on 10/30/2024.
//

#include "RigidBody.h"

#include <Engine/Window.h>
#include <Engine/WindowState.h>
#include <glm/gtx/string_cast.hpp>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

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
        _physics->Update(delta, steps, _allocator.Get(), _jobSystem.Get());

        for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
            (**i)->OnFixedUpdate(delta);
    }

    void PhysicsManager::OnEarlyFixedUpdate(float d)
    {
        OnInit();

        for(ITER_T* i = List.GetFirst(); i; i = i->GetNext())
            ((RigidBody*) &***i)->OnEarlyFixedUpdate(d);
    }

    PhysicsManager::~PhysicsManager()
    {
        px::UnregisterTypes();
        px::Factory::sInstance = nullptr;
    }

    RigidBody::RigidBody(Entity* owner, const RigidBodySettings& s, Collider& collider) :
        Component(owner, &owner->GetWindow().GetPhysics()),
        Material(s.Material),
        _settings(s),
        _collider(&collider)
    {
    }

    void RigidBody::SetMaterial(const PhysicsMaterial& material)
    {
        Material = material;

        _body->SetFriction(material.Friction);
        _body->SetRestitution(material.Bounciness);
    }

    void RigidBody::OnInit()
    {
        const Transform& transform = GetOwner().GetTransform();
        PhysicsManager& manager = GetWindow().GetPhysics();

        const px::ShapeSettings::ShapeResult result = _collider->GetShape().ScaleShape(ToPX(transform->Scale));
        GE_ASSERT(result.IsValid(), "INVALID SCALED SHAPE!");

        _previousScale = transform->Scale;

        px::BodyCreationSettings settings
        {
            result.Get(),
            px::Vec3(),
            px::Quat::sIdentity(),
            GetOwner().GetFlags().Static ? px::EMotionType::Static : px::EMotionType::Dynamic,
            (px::ObjectLayer) GetOwner().GetLayer()
        };

        settings.mUserData = (u64) this;
        settings.mFriction = _settings.Material.Friction;
        settings.mRestitution = _settings.Material.Bounciness;
        settings.mLinearDamping = _settings.LinearDamping;
        settings.mAngularDamping = _settings.AngularDamping;
        settings.mMaxLinearVelocity = _settings.TerminalVelocity;
        settings.mMaxAngularVelocity = _settings.MaxAngularVelocity;
        settings.mGravityFactor = _settings.GravityFactor;

        _body = manager._interface->CreateBody(settings);
        manager._interface->AddBody(_body->GetID(), JPH::EActivation::Activate);
    }

    void RigidBody::OnEarlyFixedUpdate(float d)
    {
        const PhysicsManager& physics = GetWindow().GetPhysics();
        const Transform& transform = GetOwner().GetTransform();
        const ColliderTransform& offset = _collider->GetTransform();

        if(!(bool)(transform._flags & TransformFlags::PhysicsInvalidated)) return;

        if(_previousScale != transform->Scale)
        {
            LOG("INFO: SHAPE SCALING");

            const px::ShapeSettings::ShapeResult result = _collider->GetShape().ScaleShape(ToPX(transform->Scale));
            GE_ASSERT(result.IsValid(), "INVALID SCALED SHAPE!");

            physics._interface->SetShape(_body->GetID(), result.Get(), true, JPH::EActivation::Activate);
            _previousScale = transform->Scale;
        }

        physics._interface->SetPositionAndRotation(
            _body->GetID(),
            ToPX(transform->Location + offset.Position * transform->Scale * transform->Rotation),
            ToPX(transform->Rotation * offset.Rotation),
            JPH::EActivation::Activate
        );
    }

    void RigidBody::OnFixedUpdate(float d)
    {
        Transform& transform = GetOwner().GetTransform();
        const ColliderTransform& offset = _collider->GetTransform();

        glm::quat rotation = ToGLM(_body->GetRotation());
        glm::vec3 location = ToGLM(_body->GetPosition()) - offset.Position * transform->Scale * inverse(rotation);

        rotation *= inverse(offset.Rotation);

        transform.SetPosition(location, TransformFlags::RenderInvalidated);
        transform.SetRotation(rotation, TransformFlags::RenderInvalidated);

        if(_body->IsStatic() || _body->IsActive()) return;

        GetOwner().Destroy();
    }

    void RigidBody::OnUpdate(float d)
    {

    }


    void RigidBody::OnDestroy()
    {
        if(!_body) return;

        PhysicsManager& manager = GetWindow().GetPhysics();

        manager._interface->RemoveBody(_body->GetID());
        manager._interface->DestroyBody(_body->GetID());

        _body = nullptr;
    }

    RigidBody::~RigidBody()
    {
        RigidBody::OnDestroy();
    }
}
