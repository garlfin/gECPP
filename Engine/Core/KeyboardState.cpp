//
// Created by scion on 11/30/2024.
//

#include "KeyboardState.h"

#include <SDL3/SDL_events.h>

#include "MouseState.h"
#include "Window.h"

namespace gE
{
    void ProcessButton(MouseButton key, KeyState& to, u32 state)
    {
        const KeyState previousState = to;

        to = state & 1 << (u32) key ? KeyState::Down : KeyState::Up;
        to |= ((to ^ previousState) & KeyState::Down) << (KeyState) 1;
    }

    void MouseState::SetPosition(glm::vec2 relative) const
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
            _mousePosition = glm::vec2(x, y);
        }
        else
        {
            SDL_SetWindowRelativeMouseMode(_window->GetSDLWindow(), true);
            SDL_HideCursor();
            state = SDL_GetRelativeMouseState(&x, &y);

            _previousMousePosition = glm::vec2(0);
            _mousePosition = glm::vec2(x, y);
        }

        for(u8 i = 0; i < 5; i++)
            ProcessButton((MouseButton) i, _buttons[i], state);
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
        const KeyState previousState = to;

        to = event.down ? KeyState::Down : KeyState::Up;
        to |= ((to ^ previousState) & KeyState::Down) << (KeyState) 1;
    }

    void KeyboardState::ClearKeyStates()
    {
        for(u16 i = 0; i < (u16) Key::Size; i++)
            _keys[i] &= KeyState::Down;
    }
}
