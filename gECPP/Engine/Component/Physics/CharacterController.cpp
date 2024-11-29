//
// Created by scion on 11/27/2024.
//

#include "CharacterController.h"

#include <Engine/Window.h>
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

        settings.mMass = 80.f; // Average person weight.
        settings.mEnhancedInternalEdgeRemoval = true;

        _controller = ptr_create<px::CharacterVirtual>(&settings, px::Vec3::sZero(), px::Quat::sIdentity(), (u64) this, &system);

        SetShape(shape);
    }

    void CharacterController::SetShape(const Physics::CapsuleShape& shape)
    {
        PhysicsManager& manager = GetWindow().GetPhysics();
        px::PhysicsSystem& system = manager.GetSystem();
        const px::PhysicsSettings& physicsSettings = system.GetPhysicsSettings();

        _shape = move(Jolt::CapsuleShape(&GetWindow(), shape));

        _controller->SetShape(
            &_shape.GetJoltShape(),
            physicsSettings.mPenetrationSlop,
            _broadFilter,
            _filter,
            DefaultBodyFilter,
            DefaultShapeFilter,
            manager.GetTempAllocator()
        );
    }

    void CharacterController::OnInit()
    {
        const Transform& transform = GetOwner().GetTransform();

        _controller->SetPosition(ToPX(transform->Position));
    }

    void CharacterController::OnUpdate(float delta)
    {

    }

    void CharacterController::OnEarlyFixedUpdate(float delta)
    {
        const PhysicsManager& manager = GetWindow().GetPhysics();
        const px::PhysicsSystem& system = manager.GetSystem();

        glm::vec3 velocity = ToGLM(_controller->GetLinearVelocity());
        velocity.y += system.GetGravity().GetY() * delta;
        velocity += _velocity - _previousVelocity;

        if(_controller->IsSupported()) velocity.y = 0.f;

        _controller->SetLinearVelocity(ToPX(velocity));
        _previousVelocity = _velocity;
    }

    void CharacterController::OnFixedUpdate(float delta)
    {
        PhysicsManager& manager = GetWindow().GetPhysics();
        const px::PhysicsSystem& system = manager.GetSystem();
        Transform& transform = GetOwner().GetTransform();

        _controller->Update(
            delta,
            system.GetGravity(),
            _broadFilter,
            _filter,
            DefaultBodyFilter,
            DefaultShapeFilter,
            manager.GetTempAllocator()
        );

        transform.SetPosition(ToGLM(_controller->GetPosition()));
    }
}
