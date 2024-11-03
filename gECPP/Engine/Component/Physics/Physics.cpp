//
// Created by scion on 10/30/2024.
//

#include "Physics.h"

#include <Engine/Window.h>
#include <Engine/WindowState.h>
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

    void PhysicsManager::OnUpdate(float delta)
    {
        EngineFlags state = GetWindow()->EngineState;

        const int steps = ceil(delta * GE_PX_MIN_TICKRATE);

        if(state.UpdateType == UpdateType::FixedUpdate)
            _physics->Update(delta, steps, _allocator.Get(), _jobSystem.Get());

        IComponentManager::OnUpdate(delta);
    }

    PhysicsManager::~PhysicsManager()
    {
        px::UnregisterTypes();
        px::Factory::sInstance = nullptr;
    }

    inline PhysicsObject::PhysicsObject(Entity* owner, const RigidBodySettings& s,  const PhysicsMaterial& material, const px::Shape& shape) :
        Component(owner, &GetWindow().GetPhysics()),
        Material(material),
        _shape(&shape),
        _density(s.Mass / shape.GetVolume())
    {
        Transform& transform = owner->GetTransform();
        PhysicsManager& manager = GetWindow().GetPhysics();

        transform.OnUpdate(0.f);

        px::MassProperties massProperties;
        massProperties.ScaleToMass(s.Mass);

        px::BodyCreationSettings settings
        {
            &shape,
            *(px::Vec3*) &transform.GetGlobalTransform().Location,
            *(px::Quat*) &transform.GetGlobalTransform().Rotation,
            px::EMotionType::Dynamic,
            (px::ObjectLayer) owner->GetLayer()
        };

        settings.mMassPropertiesOverride = massProperties;
        settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
        settings.mUserData = (u64) this;
        settings.mFriction = material.Friction;
        settings.mRestitution = material.Bounciness;
        settings.mLinearDamping = s.LinearDamping;
        settings.mAngularDamping = s.AngularDamping;
        settings.mMaxLinearVelocity = s.TerminalVelocity;
        settings.mMaxAngularVelocity = s.MaxAngularVelocity;
        settings.mGravityFactor = s.GravityFactor;

        _body = manager._interface->CreateBody(settings);
        manager._interface->AddBody(_body->GetID(), JPH::EActivation::Activate);
    }

    void PhysicsObject::SetMaterial(const PhysicsMaterial& material)
    {
        Material = material;

        _body->SetFriction(material.Friction);
        _body->SetRestitution(material.Bounciness);
    }

    inline void PhysicsObject::OnFixedUpdate(float d)
    {
        Transform& transform = GetOwner().GetTransform();

        px::Vec3 location = _body->GetPosition();
        px::Quat rotation = _body->GetRotation();

        transform.SetPosition(*(glm::vec3*) &location);
        transform.SetRotation(*(glm::quat*) &rotation);
    }

    inline void PhysicsObject::OnUpdate(float d)
    {

    }

    inline void PhysicsObject::OnRender(float d, Camera* camera)
    {

    }

    PhysicsObject::~PhysicsObject()
    {
        if(!_body) return;

        PhysicsManager& manager = GetWindow().GetPhysics();

        manager._interface->DeactivateBody(_body->GetID());
        manager._interface->RemoveBody(_body->GetID());
    }
}
