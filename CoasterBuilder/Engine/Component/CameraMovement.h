//
// Created by scion on 4/10/2025.
//

#pragma once

#include <Component/Behavior.h>

namespace Coaster
{
    class FreeCamMovement : public Behavior
    {
        REFLECTABLE_ONGUI_PROTO(Behavior);

    public:
        explicit FreeCamMovement(gE::Entity* owner) : Behavior(owner) {};

        float Speed = 1.f;
        float Friction = 10.f;
        float Sensitivity = 1.f;

        void OnInit() override {};
        void OnUpdate(float delta) override;

    private:
        float _velocity = 0.f;
        vec3 _rot = DEFAULT;
    };

    inline REFLECTABLE_ONGUI_IMPL(FreeCamMovement,
        DrawField(ScalarField{ "Speed", "", 0.1f }, Speed, depth);
        DrawField(ScalarField{ "Friction", "", 0.1f }, Friction, depth);
        DrawField(ScalarField{ "Sensitivity", "", 0.01f }, Sensitivity, depth);
    );

    inline void FreeCamMovement::OnUpdate(float delta)
    {
        Transform& transform = GetOwner().GetTransform();
        KeyboardState& keyboard = GetWindow().GetKeyboard();
        MouseState& mouse = GetWindow().GetMouse();

        vec3 direction = DEFAULT;

        if(!mouse.GetIsFocused() && IsKeyDown(mouse.GetButton(MouseButton::Right)))
        {
            _rot.y += mouse.GetDelta().x * Sensitivity;
            _rot.x += mouse.GetDelta().y * Sensitivity;
            _rot.x = std::clamp(_rot.x, -89.9f, 89.9f);

            transform.SetRotation(_rot * TO_RAD);


            if(IsKeyDown(keyboard.GetKey(Key::W))) direction.z -= 1.f;
            if(IsKeyDown(keyboard.GetKey(Key::A))) direction.x -= 1.f;
            if(IsKeyDown(keyboard.GetKey(Key::S))) direction.z += 1.f;
            if(IsKeyDown(keyboard.GetKey(Key::D))) direction.x += 1.f;

            if(direction.x + direction.y + direction.z > 0.f)
                direction = normalize(direction);

            direction = transform->Rotation * direction;
        }

        transform.SetPosition(transform->Position + direction * delta);
    }
}
