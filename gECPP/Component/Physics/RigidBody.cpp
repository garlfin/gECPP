//
// Created by scion on 11/9/2024.
//

#include "RigidBody.h"

#include <Window/Window.h>
#include <glm/gtx/string_cast.hpp>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

namespace gE
{
    RigidBody::RigidBody(Entity* owner, const RigidBodySettings& s, Collider& collider) :
        PhysicsComponent(owner, &owner->GetWindow().GetPhysics()),
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
        PhysicsComponent::OnInit();

        const Transform& transform = GetOwner().GetTransform();
        PhysicsManager& manager = GetWindow().GetPhysics();
        const px::Shape& joltShape = _collider->GetShape().GetJoltShape();

        const px::ShapeSettings::ShapeResult result = joltShape.ScaleShape(Physics::ToPX(transform->Scale));
        GE_ASSERTM(result.IsValid(), "INVALID SCALED SHAPE!");

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
        const Transform& transform = GetOwner().GetTransform();

        if((bool)(transform.GetFlags() & TransformFlags::PhysicsInvalidated))
            ForceUpdateTransforms();
    }

    void RigidBody::OnFixedUpdate(float d)
    {
        Transform& transform = GetOwner().GetTransform();
        const Physics::ColliderTransform& offset = _collider->GetTransform();

        PreviousRotation = Rotation;
        PreviousPosition = Position;

        Rotation = Physics::ToGLM(_body->GetRotation());
        Position = Physics::ToGLM(_body->GetPosition()) - offset.Position * transform->Scale * inverse(Rotation);

        Rotation *= inverse(offset.Rotation);
    }

    void RigidBody::OnDestroy()
    {
        if(!_body) return;

        PhysicsManager& manager = GetWindow().GetPhysics();

        manager._interface->RemoveBody(_body->GetID());
        manager._interface->DestroyBody(_body->GetID());

        _body = nullptr;
    }

    void RigidBody::ForceUpdateTransforms()
    {
        const PhysicsManager& physics = GetWindow().GetPhysics();
        const Transform& transform = GetOwner().GetTransform();
        const Physics::ColliderTransform& offset = _collider->GetTransform();

        if(_previousScale != transform->Scale)
        {
            Log("INFO: SHAPE SCALING");

            const px::Shape& joltShape = _collider->GetShape().GetJoltShape();
            const px::ShapeSettings::ShapeResult result = joltShape.ScaleShape(Physics::ToPX(transform->Scale));
            GE_ASSERTM(result.IsValid(), "INVALID SCALED SHAPE!");

            physics._interface->SetShape(_body->GetID(), result.Get(), true, JPH::EActivation::Activate);
            _previousScale = transform->Scale;
        }

        physics._interface->SetPositionAndRotation
        (
            _body->GetID(),
            Physics::ToPX(Position + offset.Position * transform->Scale * Rotation),
            Physics::ToPX(Rotation * offset.Rotation),
            JPH::EActivation::Activate
        );

        ResetTransformFlag();
    }

    void RigidBody::SetInstantVelocity(const glm::vec3& velocity)
    {
        px::BodyInterface& physics = *GetWindow().GetPhysics()._interface;
        physics.SetLinearVelocity(_body->GetID(), Physics::ToPX(velocity));
    }

    void RigidBody::AddImpulse(const glm::vec3& impulse)
    {
        px::BodyInterface& physics = *GetWindow().GetPhysics()._interface;
        physics.AddImpulse(_body->GetID(), Physics::ToPX(impulse));
    }

    void RigidBody::AddImpulse(const glm::vec3& impulse, const glm::vec3& dir)
    {
        px::BodyInterface& physics = *GetWindow().GetPhysics()._interface;
        physics.AddImpulse(_body->GetID(), Physics::ToPX(impulse), Physics::ToPX(dir));
    }

    void RigidBody::AddForce(const glm::vec3& force)
    {
        px::BodyInterface& physics = *GetWindow().GetPhysics()._interface;
        physics.AddForce(_body->GetID(), Physics::ToPX(force));
    }

    void RigidBody::AddForce(const glm::vec3& force, const glm::vec3& dir)
    {
        px::BodyInterface& physics = *GetWindow().GetPhysics()._interface;
        physics.AddForce(_body->GetID(), Physics::ToPX(force), Physics::ToPX(dir));
    }

    RigidBody::~RigidBody()
    {
        RigidBody::OnDestroy();
    }
}
