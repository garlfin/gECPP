//
// Created by scion on 11/30/2024.
//

#include "Input.h"

#include <SDL3/SDL_events.h>

#include "Window.h"

namespace gE
{
    void MouseState::SetPosition(vec2 relative) const
    {
        SDL_WarpMouseInWindow(_window->GetSDLWindow(), relative.x, relative.y);
    }

    void MouseState::Update()
    {
        float x, y;
        u32 state;

        if(_enabled)
        {
            SDL_SetWindowRelativeMouseMode(_window->GetSDLWindow(), false);
            SDL_ShowCursor();
            state = SDL_GetMouseState(&x, &y);

            _previousMousePosition = _mousePosition;
            _mousePosition = vec2(x, y);
        }
        else
        {
            SDL_SetWindowRelativeMouseMode(_window->GetSDLWindow(), true);
            SDL_HideCursor();
            state = SDL_GetRelativeMouseState(&x, &y);

            _previousMousePosition = vec2(0);
            _mousePosition = vec2(x, y);
        }

        for(u8 i = 0; i < 5; i++)
            UpdateKeyState(state >> i & 1, _buttons[i]);
    }

    void KeyboardState::ProcessKey(const SDL_KeyboardEvent& event)
    {
        const SDL_Scancode scanCode = SDL_GetScancodeFromKey(event.key, nullptr);

        if(scanCode >= (u16) Key::Size)
        {
    #ifdef DEBUG
            Log::Write("Discarded key: {}\n", SDL_GetScancodeName(scanCode));
    #endif
            return;
        }

        KeyState& to = _keys[scanCode];
        UpdateKeyState(event.down, to);

        if(!IsKeyDown(to)) return;
        if(scanCode < (SDL_Scancode) KeyModifier::Min)
        {
            if(to != KeyState::Pressed) return;
            if(!(bool) _shortcut.Third)
                _shortcut.Third = (Key) scanCode;
        }
        else
        {
            if(!(bool) _shortcut.First)
                _shortcut.First = (KeyModifier) scanCode;
            else if(!(bool) _shortcut.Second)
                _shortcut.Second = (KeyModifier) scanCode;
        }
    }

    void KeyboardState::ClearShortcutState()
    {
        if(!IsKeyDown(GetKey(_shortcut.First))) _shortcut.First = KeyModifier::None;
        if(!IsKeyDown(GetKey(_shortcut.Second))) _shortcut.Second = KeyModifier::None;
    }

    void KeyboardState::ClearKeyStates()
    {
        for(u16 i = 0; i < (u16) Key::Size; i++)
            _keys[i] &= KeyState::Down;

        _shortcut.Third = Key::None;
    }

    ControllerState::ControllerState(Window* window, u32 id) :
        _window(window)
    {
        _gamepad = SDL_OpenGamepad(id);
    }

    float IControllerState::GetLeftTrigger(const ControllerStickMapping& map) const
    {
        return map.Apply(LeftTrigger);
    }

    float IControllerState::GetRightTrigger(const ControllerStickMapping& map) const
    {
        return map.Apply(RightTrigger);
    }

    ControllerStickState IControllerState::GetLeftStick(const ControllerStickMapping& map) const
    {
        return { map.Apply(LeftStick), Buttons[(u8) ControllerButton::LeftStick] };
    }

    ControllerStickState IControllerState::GetRightStick(const ControllerStickMapping& map) const
    {
        return { map.Apply(RightStick), Buttons[(u8) ControllerButton::RightStick] };
    }

#define GET_STICK(STICK) (float) SDL_GetGamepadAxis(_gamepad, STICK) / INT16_MAX
    void ControllerState::Update()
    {
        LeftTrigger = GET_STICK(SDL_GAMEPAD_AXIS_LEFT_TRIGGER);
        RightTrigger = GET_STICK(SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);

        LeftStick.x = GET_STICK(SDL_GAMEPAD_AXIS_LEFTX);
        LeftStick.y = GET_STICK(SDL_GAMEPAD_AXIS_LEFTY);

        RightStick.x = GET_STICK(SDL_GAMEPAD_AXIS_RIGHTX);
        RightStick.y = GET_STICK(SDL_GAMEPAD_AXIS_RIGHTY);

        for(u8 i = 0; i < (u8) ControllerButton::Size; i++)
            UpdateKeyState(SDL_GetGamepadButton(_gamepad, (SDL_GamepadButton) i), Buttons[i]);
    }
#undef GET_STICK
}
