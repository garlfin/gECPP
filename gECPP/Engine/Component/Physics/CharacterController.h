//
// Created by scion on 11/27/2024.
//

#pragma once

#include <Engine/Component/Component.h>

#include <Physics/Physics.h>
#include <Physics/Shapes.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Engine/Utility/AssetManager.h>

#include "RigidBody.h"

namespace gE
{
    class CharacterController : public PhysicsComponent
    {
    public:
        CharacterController(Entity* owner, const Physics::CapsuleShape&);

        void OnInit() override;
        void OnEarlyFixedUpdate(float delta) override;
        void OnFixedUpdate(float delta) override;
        void OnUpdate(float delta) override;

        GET_CONST(const Jolt::CapsuleShape&, Shape, _shape);
        GET_CONST(const px::CharacterVirtual&, JoltController, *_controller);

        GET_CONST(float, Mass, _controller->GetMass());
        void SetMass(float mass) { _controller->SetMass(mass); }

        GET_SET_VALUE(bool, UseGravity, _useGravity);
        GET_SET_VALUE(glm::vec3, Velocity, _velocity);

        void SetShape(const Physics::CapsuleShape& shape);

    private:
        SmartPointer<px::CharacterVirtual> _controller;
        Jolt::CapsuleShape _shape;

        UCollisionFilter _filter;
        UBroadPhaseFilter _broadFilter;

        bool _useGravity = true;
        glm::vec3 _velocity = DEFAULT;
        glm::vec3 _previousVelocity = DEFAULT;
    };
}
