//
// Created by scion on 11/30/2024.
//

#pragma once

#include <Prototype.h>
#include <Core/Macro.h>
#include <Core/Math/Math.h>

#include "KeyboardState.h"

namespace gE
{
    enum class MouseButton : u8
    {
        Left = 0,
        Middle = 1,
        Right = 2,
        Front = 3,
        Back = 4,
        Last = Back
    };

    class MouseState
    {
    public:
        explicit MouseState(Window* window) : _window(window) {};

        GET_SET(bool, IsEnabled, _enabled);
        GET_SET(bool, IsFocused, _focused);
        GET_CONST(glm::vec2, Position, _mousePosition);
        GET_CONST(glm::vec2, Delta, _previousMousePosition - _mousePosition);

        NODISCARD KeyState GetButton(MouseButton button) const { return _buttons[(u8) button]; }

        void SetPosition(glm::vec2) const;

        void Update();

    private:
        Window* _window;

        glm::vec2 _mousePosition = DEFAULT;
        glm::vec2 _previousMousePosition = DEFAULT;
        KeyState _buttons[(u8) MouseButton::Last + 1] = DEFAULT;
        bool _enabled = true;
        bool _focused = false;
    };
};