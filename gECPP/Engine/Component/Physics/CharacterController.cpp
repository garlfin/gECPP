//
// Created by scion on 11/27/2024.
//

#include "CharacterController.h"

#include <Engine/Window/Window.h>
#include <glm/gtx/string_cast.hpp>

namespace gE
{
    CharacterController::CharacterController(Entity* owner, const Physics::CapsuleShape& shape) :
        PhysicsComponent(owner, &owner->GetWindow().GetPhysics()),
        _filter(GetOwner().GetLayer()),
        _broadFilter(GetOwner().GetLayer())
    {
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

        GE_ASSERT(result, "COULD NOT CHANGE SHAPE!");

        _controller->SetInnerBodyShape(&newShape.GetJoltShape());

        _shape = move(newShape);
    }

    void CharacterController::OnInit()
    {
        PhysicsComponent::OnInit();

        const Transform& transform = GetOwner().GetTransform();
        _controller->SetPosition(ToPX(transform->Position));

        _velocity = glm::vec3(0.f);
    }

    void CharacterController::OnEarlyFixedUpdate(float delta)
    {
        const PhysicsManager& manager = GetWindow().GetPhysics();
        const px::PhysicsSystem& system = manager.GetSystem();
        const Transform& transform = GetOwner().GetTransform();

        glm::vec3 velocity = _velocity;
        velocity.y += system.GetGravity().GetY();

        if(_controller->IsSupported() || !_useGravity) velocity.y = 0.f;

        _controller->SetLinearVelocity(ToPX(velocity));

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

        PreviousPosition = Position;
        Position = ToGLM(_controller->GetPosition());
    }

    void CharacterController::ForceUpdateTransforms()
    {
        const Transform& transform = GetOwner().GetTransform();

        ResetTransformFlag();

        Position = transform->Position;
        Rotation = transform->Rotation;

        _controller->SetPosition(ToPX(Position));
        _controller->SetRotation(ToPX(Rotation));
    }
}
