//
// Created by scion on 11/27/2024.
//

#include "CharacterController.h"

#include <Window.h>
#include <glm/gtx/string_cast.hpp>

namespace gE
{
    CharacterController::CharacterController(Entity* owner, const Physics::CapsuleShape& shape) :
        PhysicsComponent(owner, &owner->GetWindow().GetPhysics()),
        _filter(GetOwner().GetLayer()),
        _broadFilter(GetOwner().GetLayer())
    {
        UseRotation = false;

        PhysicsManager& manager = GetWindow().GetPhysics();
        px::PhysicsSystem& system = manager.GetSystem();

        JPH::CharacterVirtualSettings settings = DEFAULT;

        settings.mMass = shape.Mass;
        settings.mEnhancedInternalEdgeRemoval = true;
        settings.mInnerBodyLayer = (px::ObjectLayer) GetOwner().GetLayer();

        _controller = ptr_create<px::CharacterVirtual>(&settings, px::Vec3::sZero(), px::Quat::sIdentity(), (u64) this, &system);

        SetShape(shape);
    }

    void CharacterController::SetShape(const Physics::CapsuleShape& shape)
    {
        PhysicsManager& manager = GetWindow().GetPhysics();

        if(_shape && _shape.GetSettings() == shape) return;

        auto newShape = Jolt::CapsuleShape(&GetWindow(), shape);

        _controller->SetMass(shape.Mass);
        bool result = _controller->SetShape(
            &newShape.GetJoltShape(),
            FLT_MAX,
            _broadFilter,
            _filter,
            DefaultBodyFilter,
            DefaultShapeFilter,
            manager.GetTempAllocator()
        );

        GE_ASSERTM(result, "COULD NOT CHANGE SHAPE!");

        _controller->SetInnerBodyShape(&newShape.GetJoltShape());

        _shape = move(newShape);
    }

    void CharacterController::OnInit()
    {
        PhysicsComponent::OnInit();

        const Transform& transform = GetOwner().GetTransform();
        _controller->SetPosition(Physics::ToPX(transform->Position));

        _velocity = DEFAULT;
        _instantVelocity = DEFAULT;
    }

    void CharacterController::OnEarlyFixedUpdate(float delta)
    {
        const PhysicsManager& manager = GetWindow().GetPhysics();
        const px::PhysicsSystem& system = manager.GetSystem();
        const Transform& transform = GetOwner().GetTransform();

        if(_useGravity)
        {
            if(GetIsGrounded())
                _velocity.y = std::max(_velocity.y, 0.f);
            else
                _velocity.y += system.GetGravity().GetY() * delta;
        }

        _controller->SetLinearVelocity(Physics::ToPX(_velocity + _instantVelocity / delta));

        if((bool)(transform.GetFlags() & TransformFlags::PhysicsInvalidated))
            ForceUpdateTransforms();
    }

    void CharacterController::OnFixedUpdate(float delta)
    {
        PhysicsManager& manager = GetWindow().GetPhysics();
        const px::PhysicsSystem& system = manager.GetSystem();

        _controller->Update(
            delta,
            system.GetGravity(),
            _broadFilter,
            _filter,
            DefaultBodyFilter,
            DefaultShapeFilter,
            manager.GetTempAllocator()
        );

        _grounded = _controller->GetGroundState() == JPH::CharacterBase::EGroundState::OnGround;

        if(_grounded && _useGravity)
            _velocity = DEFAULT;
        _instantVelocity = DEFAULT;

        PreviousPosition = Position;

        Position = Physics::ToGLM(_controller->GetPosition());
    }

    void CharacterController::ForceUpdateTransforms()
    {
        const Transform& transform = GetOwner().GetTransform();

        Position = transform->Position;

        _controller->SetPosition(Physics::ToPX(Position));
    }
}
