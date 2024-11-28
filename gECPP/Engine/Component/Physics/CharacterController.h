//
// Created by scion on 11/27/2024.
//

#pragma once

#include <Engine/Component/Component.h>

#include <Physics/Physics.h>
#include <Physics/Shapes.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Engine/Utility/AssetManager.h>

namespace gE
{
    class CharacterController : public Component
    {
    public:
        CharacterController(Entity* owner, const Physics::CapsuleShape&);

        GET_CONST(const Jolt::CapsuleShape&, Shape, _shape);
        GET_CONST(const px::CharacterVirtual&, JoltController, *_controller);

        GET_CONST(float, Mass, _controller->GetMass());
        void SetMass(float mass) { _controller->SetMass(mass); }

    private:
        SmartPointer<px::CharacterVirtual> _controller;
        Jolt::CapsuleShape _shape;

        UCollisionFilter _filter;
        UBroadPhaseFilter _broadFilter;
    };
}
