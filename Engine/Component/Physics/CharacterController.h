//
// Created by scion on 11/27/2024.
//

#pragma once

#include <Component/Component.h>

#include <Core/AssetManager.h>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Physics/Physics.h>
#include <Physics/Shapes.h>

#include "Physics.h"

namespace gE
{
    class CharacterController final : public PhysicsComponent
    {
    public:
        CharacterController(Entity* owner, const Physics::CapsuleShape&);

        void OnInit() override;
        void OnEarlyFixedUpdate(float delta) override;
        void OnFixedUpdate(float delta) override;

        void ForceUpdateTransforms() override;

        GET_CONST(const Jolt::CapsuleShape&, Shape, _shape)
        GET_CONST(const px::CharacterVirtual&, JoltController, *_controller);

        GET_CONST(float, Mass, _controller->GetMass());
        ALWAYS_INLINE void SetMass(float mass) { _controller->SetMass(mass); }

        GET_SET_VALUE(bool, UseGravity, _useGravity);
        GET_SET_VALUE(glm::vec3, Velocity, _velocity);

        ALWAYS_INLINE void AddVelocity(const glm::vec3& velocity) { _velocity += velocity; }
        ALWAYS_INLINE void Move(const glm::vec3& position) { _instantVelocity += position; }

        GET_SET_VALUE(bool, IsGrounded, _grounded);
        NODISCARD PhysicsComponent* GetGround() const { return (PhysicsComponent*) _controller->GetGroundUserData(); }

        void SetShape(const Physics::CapsuleShape& shape);

    private:
        Pointer<px::CharacterVirtual> _controller;
        Jolt::CapsuleShape _shape;

        UCollisionFilter _filter;
        UBroadPhaseFilter _broadFilter;

        bool _useGravity = true;
        glm::vec3 _velocity = DEFAULT;
        glm::vec3 _instantVelocity = DEFAULT;
        bool _grounded = false;
    };
}
