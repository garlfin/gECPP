//
// Created by scion on 9/13/2023.
//

#pragma once

#include <Window.h>
#include <Component/Behavior.h>
#include <Component/Physics/CharacterController.h>
#include <Entity/Entity.h>
#include <glm/gtx/string_cast.hpp>

namespace gE
{
    class Movement final : public Behavior
    {
        SERIALIZABLE_PROTO(Movement, Behavior);

    public:
        explicit Movement(Entity* o, CharacterController& controller) :
            Behavior(o),
            _controller(controller)
        {}

        GET_SET(Entity*, FPCamera, _camera);

        float Speed = 2.0;
        float SpeedMultiplier = 2.f;
        float StandingHeight = 1.75;
        float CrouchingHeight = 0.875;
        float JumpHeight = 1.f;
        float Sensitivity = 0.1f;

        void OnInit() override
        {
            GetWindow().GetMouse().SetIsEnabled(false);
        }

        void OnUpdate(float delta) override
        {
            Transform& transform = GetOwner().GetTransform();
            Transform& cameraTransform = _camera->GetTransform();

            MouseState& mouse = GetWindow().GetMouse();
            const KeyboardState& keyboard = GetWindow().GetKeyboard();
            const KeyState crouchState = keyboard.GetKey(Key::C);

            const bool grounded = _controller->GetIsGrounded();
            const bool cursorEnabled = mouse.GetIsEnabled();

            if (keyboard.GetKey(Key::Escape) == KeyState::Pressed)
            {
                mouse.SetIsEnabled(!cursorEnabled);
                mouse.SetPosition(GetWindow().GetSize() / 2u);
            }

            if(GetWindow().GetVREnabled())
                cameraTransform.SetPosition(cameraTransform->Position - StandingHeight / 2.f * vec3(0, 1, 0));

            if (cursorEnabled) return;

            _rot.y += mouse.GetDelta().x * Sensitivity;
            _rot.x += mouse.GetDelta().y * Sensitivity;
            _rot.x = std::clamp(_rot.x, -89.9f, 89.9f);

            transform.SetRotation(vec3(0, _rot.y * TO_RAD, 0));
            if(!GetWindow().GetVREnabled()) cameraTransform.SetRotation(vec3(_rot.x * TO_RAD, 0, 0));

            vec3 dir(0.f);
            if (IsKeyDown(keyboard.GetKey(Key::W))) dir.z -= 1.f;
            if (IsKeyDown(keyboard.GetKey(Key::S))) dir.z += 1.f;
            if (IsKeyDown(keyboard.GetKey(Key::A))) dir.x -= 1.f;
            if (IsKeyDown(keyboard.GetKey(Key::D))) dir.x += 1.f;

            if (length2(dir) > 0) dir = normalize(dir);

            Physics::CapsuleShape capsuleShape = DEFAULT;
            capsuleShape.Height = StandingHeight;

            if (IsKeyDown(crouchState))
            {
                capsuleShape.Height = CrouchingHeight;
                dir *= 0.5;
            }

            if ((bool)(crouchState & KeyState::StateChanged))
            {
                const float heightDifference = (StandingHeight - CrouchingHeight) / 2.f;
                if (IsKeyDown(crouchState))
                    transform.SetPosition(transform->Position - vec3(0, heightDifference, 0));
                else
                    transform.SetPosition(transform->Position + vec3(0, heightDifference, 0));
                _controller->ForceUpdateTransforms();
            }

            if(!GetWindow().GetVREnabled()) cameraTransform.SetPosition(vec3(0, capsuleShape.Height / 2.f, 0));
            _controller->SetShape(capsuleShape);

            dir *= Speed;
            if (!IsKeyDown(crouchState) && IsKeyDown(keyboard.GetKey(KeyModifier::LShift))) dir *= SpeedMultiplier;

            if (grounded) _dir = transform->Rotation * dir;

            _controller->Move(_dir * delta);

            if (IsKeyDown(keyboard.GetKey(Key::Space)) && grounded)
            {
                _controller->AddVelocity(vec3(0, std::sqrt(2.f * 9.81 * JumpHeight), 0));
                _controller->SetIsGrounded(false);
            }
        }

    private:
        vec3 _rot = DEFAULT;
        RelativePointer<CharacterController> _controller;
        Entity* _camera = DEFAULT;
        vec3 _dir = DEFAULT;
    };

    inline void Movement::IDeserialize(istream& in, SETTINGS_T s)
    {
        Read(in, Speed);
        Read(in, SpeedMultiplier);
        Read(in, StandingHeight);
        Read(in, CrouchingHeight);
        Read(in, JumpHeight);
        Read(in, Sensitivity);
        Read(in, _controller);
    }

    inline void Movement::ISerialize(ostream& out) const
    {
        Write(out, Speed);
        Write(out, SpeedMultiplier);
        Write(out, StandingHeight);
        Write(out, CrouchingHeight);
        Write(out, JumpHeight);
        Write(out, Sensitivity);
        Write(out, _controller);
    }

    inline REFLECTABLE_ONGUI_IMPL(Movement,
    {
        DrawField(ScalarField{ "Speed"sv, ""sv, 0.f, 10.f, 0.01f }, Speed, depth);
        DrawField(ScalarField{ "Speed Multiplier"sv, ""sv, 0.f, 10.f, 0.01f }, SpeedMultiplier, depth);
        DrawField(ScalarField{ "Height"sv, ""sv, 0.f, 10.f, 0.01f }, StandingHeight, depth);
        DrawField(ScalarField{ "Crouching Height"sv, ""sv, 0.f, 10.f, 0.01f }, CrouchingHeight, depth);
        DrawField(ScalarField{ "Jump Height"sv, ""sv, 0.f, 10.f, 0.01f }, JumpHeight, depth);
        DrawField(ScalarField{ "Sensitivity"sv, "Mouse sensitivity"sv, 0.01f, 10.f, 0.001f }, Sensitivity, depth);
    });

    REFLECTABLE_FACTORY_IMPL(Movement, inline);
}
