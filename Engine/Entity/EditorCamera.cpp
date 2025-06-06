//
// Created by scion on 6/4/2025.
//

#include <Engine/Window.h>
#include "EditorCamera.h"

namespace gE
{
    void EditorCameraMovement::OnInit()
    {
    }

    void EditorCameraMovement::OnUpdate(float delta)
    {
        if(GetWindow().GetEditor()->GetIsRunning())
            return;

        Transform& transform = GetOwner().GetTransform();

        MouseState& mouse = GetWindow().GetMouse();
        const KeyboardState& keyboard = GetWindow().GetKeyboard();

        const KeyState rightClick = mouse.GetButton(MouseButton::Right);

        if(rightClick == KeyState::Pressed)
            _mousePos = mouse.GetPosition();
        if (IsKeyDown(rightClick))
        {
            mouse.SetIsEnabled(false);
            mouse.SetPosition(_mousePos);
        }
        else
        {
            mouse.SetIsEnabled(true);
            return;
        }

        _rot.y += mouse.GetDelta().x * Sensitivity;
        _rot.x += mouse.GetDelta().y * Sensitivity;
        _rot.x = std::clamp(_rot.x, -89.9f, 89.9f);

        transform.SetRotation(vec3(_rot.x, _rot.y, 0) * TO_RAD);

        vec3 dir(0.f);
        if (IsKeyDown(keyboard.GetKey(Key::W))) dir.z -= 1.f;
        if (IsKeyDown(keyboard.GetKey(Key::S))) dir.z += 1.f;
        if (IsKeyDown(keyboard.GetKey(Key::A))) dir.x -= 1.f;
        if (IsKeyDown(keyboard.GetKey(Key::D))) dir.x += 1.f;
        if (IsKeyDown(keyboard.GetKey(Key::E))) dir.y += 1.f;
        if (IsKeyDown(keyboard.GetKey(Key::Q))) dir.y -= 1.f;

        if (length2(dir) > 0)
            dir = normalize(dir);

        dir *= Speed;
        if (IsKeyDown(keyboard.GetKey(KeyModifier::LShift)))
            dir *= SpeedMultiplier;
        dir = transform->Rotation * dir;

        transform.SetPosition(transform->Position + dir * delta);
    }

    EditorCamera::EditorCamera(Window* window) : Entity(window, nullptr, LayerMask::All, EntityFlags::Internal),
        _movement(this),
        _camera(this, _target, {{ FlyCameraSettings, window->GetSize() }}, &window->GetCameras()),
        _target(*this, _camera, { &_tonemap }),
        _tonemap(&_target, &_physicalCamera)
    {
        SetName("bobs your uncle");
    }
}
