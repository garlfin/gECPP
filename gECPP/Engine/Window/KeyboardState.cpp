//
// Created by scion on 11/30/2024.
//

#include "KeyboardState.h"
#include "MouseState.h"

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

namespace gE
{
    inline void ProcessKey(Key key, KeyState& to, GLFWwindow* window)
    {
        const KeyState previousState = to;

        to = glfwGetKey(window, (int) key) ? KeyState::Down : KeyState::Up;
        to |= ((to ^ previousState) & KeyState::Down) << (KeyState) 1;
    }

    inline void ProcessButton(MouseButton key, KeyState& to, GLFWwindow* window)
    {
        const KeyState previousState = to;

        to = glfwGetMouseButton(window, (u8) key) ? KeyState::Down : KeyState::Up;
        to |= ((to ^ previousState) & KeyState::Down) << (KeyState) 1;
    }

    void KeyboardState::Update(GLFWwindow* window)
    {
        ProcessKey(Key::Space, _keys[GLFW_KEY_SPACE], window);
        ProcessKey(Key::Apostraphe, _keys[GLFW_KEY_APOSTROPHE], window);

        for(u16 i = GLFW_KEY_COMMA; i <= GLFW_KEY_9; i++)
            ProcessKey((Key) i, _keys[i], window);

        ProcessKey(Key::Semicolon, _keys[GLFW_KEY_SEMICOLON], window);
        ProcessKey(Key::Equals, _keys[GLFW_KEY_EQUAL], window);

        for(u16 i = GLFW_KEY_A; i <= GLFW_KEY_RIGHT_BRACKET; i++)
            ProcessKey((Key) i, _keys[i], window);

        ProcessKey(Key::BackTick, _keys[GLFW_KEY_GRAVE_ACCENT], window);

        for(u16 i = GLFW_KEY_ESCAPE; i <= GLFW_KEY_END; i++)
            ProcessKey((Key) i, _keys[i], window);

        for(u16 i = GLFW_KEY_CAPS_LOCK; i <= GLFW_KEY_PAUSE; i++)
            ProcessKey((Key) i, _keys[i], window);

        for(u16 i = GLFW_KEY_F1; i <= GLFW_KEY_F25; i++)
            ProcessKey((Key) i, _keys[i], window);

        for(u16 i = GLFW_KEY_KP_0; i <= GLFW_KEY_KP_EQUAL; i++)
            ProcessKey((Key) i, _keys[i], window);

        for(u16 i = GLFW_KEY_LEFT_SHIFT; i <= GLFW_KEY_RIGHT_SUPER; i++)
            ProcessKey((Key) i, _keys[i], window);
    }

    void MouseState::Update(GLFWwindow* window)
    {
        for(u8 i = GLFW_MOUSE_BUTTON_1; i <= GLFW_MOUSE_BUTTON_8; i++)
            ProcessButton((MouseButton) i, _buttons[i], window);

        double x, y;
        glfwGetCursorPos(window, &x, &y);

        _previousMousePosition = _mousePosition;
        _mousePosition = glm::vec2(-x, -y);
    }
}
