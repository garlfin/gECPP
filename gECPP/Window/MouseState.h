//
// Created by scion on 11/30/2024.
//

#pragma once

#include <Prototype.h>
#include <Math/Math.h>
#include <Utility/Macro.h>

#include "KeyboardState.h"

namespace gE
{
    enum class MouseButton : u8
    {
        Left,
        Right,
        Middle,
        Four,
        Five,
        Six,
        Seven,
        Eight,
        Last = Eight
    };

    class MouseState
    {
    public:
        MouseState() = default;

        NODISCARD glm::vec2 GetPosition() const { return _mousePosition; }
        NODISCARD glm::vec2 GetDelta() const { return _mousePosition - _previousMousePosition; }
        NODISCARD KeyState GetButton(MouseButton button) const { return _buttons[(u8) button]; }

        void Update(Window& window);

    private:
        glm::vec2 _mousePosition = DEFAULT;
        glm::vec2 _previousMousePosition = DEFAULT;
        KeyState _buttons[(u8) MouseButton::Last + 1] = DEFAULT;
    };
};