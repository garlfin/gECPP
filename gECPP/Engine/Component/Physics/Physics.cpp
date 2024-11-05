//
// Created by scion on 10/30/2024.
//

#include "Physics.h"

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

    void PhysicsManager::OnUpdate(float delta)
    {
        EngineFlags state = GetWindow()->EngineState;

        const int steps = std::min<int>(ceil(delta * GE_PX_MIN_TICKRATE), GE_PX_MAX_STEPS);

        if(state.UpdateType == UpdateType::FixedUpdate)
            _physics->Update(delta, steps, _allocator.Get(), _jobSystem.Get());

        IComponentManager::OnUpdate(delta);
    }

    PhysicsManager::~PhysicsManager()
    {
        px::UnregisterTypes();
        px::Factory::sInstance = nullptr;
    }

    RigidBody::RigidBody(Entity* owner, const RigidBodySettings& s, const px::Shape& shape) :
        Component(owner, &owner->GetWindow().GetPhysics()),
        Material(s.Material),
        _shape(&shape),
        _settings(s)
    {
    }

    void RigidBody::FinalizeConstruction()
    {
        PhysicsManager& manager = GetWindow().GetPhysics();

        px::BodyCreationSettings settings
        {
            _shape,
            px::Vec3(0.f, 0.f, 0.f),
            px::Quat(0.f, 0.f, 0.f, 1.f),
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

    void RigidBody::SetMaterial(const PhysicsMaterial& material)
    {
        Material = material;

        _body->SetFriction(material.Friction);
        _body->SetRestitution(material.Bounciness);
    }

    void RigidBody::OnInit()
    {
        PhysicsManager& physics = GetWindow().GetPhysics();
        Transform& transform = GetOwner().GetTransform();

        physics._interface->SetPositionAndRotation(
            _body->GetID(),
            ToPX(transform->Location),
            ToPX(transform->Rotation),
            JPH::EActivation::Activate
        );
    }

    inline void RigidBody::OnFixedUpdate(float d)
    {
        Transform& transform = GetOwner().GetTransform();

        px::Vec3 location = _body->GetPosition();
        px::Quat rotation = _body->GetRotation();

        transform.SetPosition(*(glm::vec3*) &location);
        transform.SetRotation(*(glm::quat*) &rotation);
    }

    inline void RigidBody::OnUpdate(float d)
    {

    }

    inline void RigidBody::OnRender(float d, Camera* camera)
    {

    }

    RigidBody::~RigidBody()
    {
        if(!_body) return;

        PhysicsManager& manager = GetWindow().GetPhysics();

        manager._interface->DeactivateBody(_body->GetID());
        manager._interface->RemoveBody(_body->GetID());
    }
}
