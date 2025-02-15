//
// Created by scion on 9/13/2023.
//

#pragma once

#include <Component/Behavior.h>
#include <Component/Physics/CharacterController.h>
#include <Entity/Entity.h>
#include <gECPP/Window/Window.h>
#include <glm/gtx/string_cast.hpp>

#define SENSITIVITY 0.1f
#define SPEED_MULTIPLIER 2.f
#define JUMP_HEIGHT 1.f

namespace gE::VoxelDemo
{
    class Movement : public Behavior
    {
        REFLECTABLE_PROTO(Movement, Behavior, "gE::Movement");

    public:
        explicit Movement(Entity* o, CharacterController& controller) :
            Behavior(o),
            _controller(controller)
        {
            GetWindow().GetMouse().SetIsEnabled(false);
        }

        GET_SET_VALUE(Entity*, FPCamera, _camera);

        float Speed = 2.0;
        float StandingHeight = 1.75;
        float CrouchingHeight = 0.875;

        void OnInit() override {};

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

            if (cursorEnabled) return;

            _rot.y += mouse.GetDelta().x * SENSITIVITY;
            _rot.x += mouse.GetDelta().y * SENSITIVITY;
            _rot.x = std::clamp(_rot.x, -89.9f, 89.9f);

            transform.SetRotation(glm::vec3(0, _rot.y * TO_RAD, 0));
            cameraTransform.SetRotation(glm::vec3(_rot.x * TO_RAD, 0, 0));

            glm::vec3 dir(0.f);
            if (IsKeyDown(keyboard.GetKey(Key::W))) dir.z -= 1.f;
            if (IsKeyDown(keyboard.GetKey(Key::S))) dir.z += 1.f;
            if (IsKeyDown(keyboard.GetKey(Key::A))) dir.x -= 1.f;
            if (IsKeyDown(keyboard.GetKey(Key::D))) dir.x += 1.f;

            if (glm::length2(dir) > 0) dir = normalize(dir);

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
                    transform.SetPosition(transform->Position - glm::vec3(0, heightDifference, 0));
                else
                    transform.SetPosition(transform->Position + glm::vec3(0, heightDifference, 0));
                _controller->ForceUpdateTransforms();
            }

            cameraTransform.SetPosition(glm::vec3(0, capsuleShape.Height / 2.f, 0));
            _controller->SetShape(capsuleShape);

            dir *= Speed;
            if (!IsKeyDown(crouchState) && IsKeyDown(keyboard.GetKey(Key::LShift))) dir *= SPEED_MULTIPLIER;

            if (grounded) _dir = transform->Rotation * dir;

            _controller->Move(_dir * delta);

            if (IsKeyDown(keyboard.GetKey(Key::Space)) && grounded)
            {
                _controller->AddVelocity(glm::vec3(0, std::sqrt(2.f * 9.81 * JUMP_HEIGHT), 0));
                _controller->SetIsGrounded(false);
            }
        }

    private:
        glm::vec3 _rot = DEFAULT;
        RelativePointer<CharacterController> _controller;
        Entity* _camera;
        glm::vec3 _dir = DEFAULT;
    };

    inline REFLECTABLE_FACTORY_NO_IMPL(Movement);

    inline void Movement::IOnEditorGUI(u8 depth)
    {
        REFLECT_FIELD(Speed);
        REFLECT_FIELD(StandingHeight);
        REFLECT_FIELD(CrouchingHeight);
    }
}
