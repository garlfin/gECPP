//
// Created by scion on 11/30/2024.
//

#pragma once

#include <Core/Macro.h>
#include <Core/Math/Math.h>
#include <SDL3/SDL_events.h>

struct GLFWwindow;

namespace gE
{
    enum class Key : u16
    {
        None = 0,
        Space = SDL_SCANCODE_SPACE,
        Apostraphe = SDL_SCANCODE_APOSTROPHE,
        Comma = SDL_SCANCODE_COMMA,
        Minus = SDL_SCANCODE_MINUS,
        Period = SDL_SCANCODE_PERIOD,
        Slash = SDL_SCANCODE_SLASH,
        Zero = SDL_SCANCODE_0,
        One = SDL_SCANCODE_1,
        Two = SDL_SCANCODE_2,
        Three = SDL_SCANCODE_3,
        Four = SDL_SCANCODE_4,
        Five = SDL_SCANCODE_5,
        Six = SDL_SCANCODE_6,
        Seven = SDL_SCANCODE_7,
        Eight = SDL_SCANCODE_8,
        Nine = SDL_SCANCODE_9,
        Semicolon = SDL_SCANCODE_SEMICOLON,
        Equals = SDL_SCANCODE_EQUALS,
        A = SDL_SCANCODE_A,
        B = SDL_SCANCODE_B,
        C = SDL_SCANCODE_C,
        D = SDL_SCANCODE_D,
        E = SDL_SCANCODE_E,
        F = SDL_SCANCODE_F,
        G = SDL_SCANCODE_G,
        H = SDL_SCANCODE_H,
        I = SDL_SCANCODE_I,
        J = SDL_SCANCODE_J,
        K = SDL_SCANCODE_K,
        L = SDL_SCANCODE_L,
        M = SDL_SCANCODE_M,
        N = SDL_SCANCODE_N,
        O = SDL_SCANCODE_O,
        P = SDL_SCANCODE_P,
        Q = SDL_SCANCODE_Q,
        R = SDL_SCANCODE_R,
        S = SDL_SCANCODE_S,
        T = SDL_SCANCODE_T,
        U = SDL_SCANCODE_U,
        V = SDL_SCANCODE_V,
        W = SDL_SCANCODE_W,
        X = SDL_SCANCODE_X,
        Y = SDL_SCANCODE_Y,
        Z = SDL_SCANCODE_Z,
        LeftBracket = SDL_SCANCODE_LEFTBRACKET,
        BackSlash = SDL_SCANCODE_BACKSLASH,
        RightBracket = SDL_SCANCODE_RIGHTBRACKET,
        BackTick = SDL_SCANCODE_GRAVE,
        Escape = SDL_SCANCODE_ESCAPE,
        Enter = SDL_SCANCODE_RETURN,
        Tab = SDL_SCANCODE_TAB,
        Backspace = SDL_SCANCODE_BACKSPACE,
        Insert = SDL_SCANCODE_INSERT,
        Delete = SDL_SCANCODE_DELETE,
        Right = SDL_SCANCODE_RIGHT,
        Left = SDL_SCANCODE_LEFT,
        Down = SDL_SCANCODE_DOWN,
        Up = SDL_SCANCODE_UP,
        PageUp = SDL_SCANCODE_PAGEUP,
        PageDown = SDL_SCANCODE_PAGEDOWN,
        Home = SDL_SCANCODE_HOME,
        End = SDL_SCANCODE_END,
        CapsLock = SDL_SCANCODE_CAPSLOCK,
        ScrollLock = SDL_SCANCODE_SCROLLLOCK,
        NumLock = SDL_SCANCODE_NUMLOCKCLEAR,
        Print = SDL_SCANCODE_PRINTSCREEN,
        Pause = SDL_SCANCODE_PAUSE,
        F1 = SDL_SCANCODE_F1,
        F2 = SDL_SCANCODE_F2,
        F3 = SDL_SCANCODE_F3,
        F4 = SDL_SCANCODE_F4,
        F5 = SDL_SCANCODE_F5,
        F6 = SDL_SCANCODE_F6,
        F7 = SDL_SCANCODE_F7,
        F8 = SDL_SCANCODE_F8,
        F9 = SDL_SCANCODE_F9,
        F10 = SDL_SCANCODE_F10,
        F11 = SDL_SCANCODE_F11,
        F12 = SDL_SCANCODE_F12,
        F13 = SDL_SCANCODE_F13,
        F14 = SDL_SCANCODE_F14,
        F15 = SDL_SCANCODE_F15,
        F16 = SDL_SCANCODE_F16,
        F17 = SDL_SCANCODE_F17,
        F18 = SDL_SCANCODE_F18,
        F19 = SDL_SCANCODE_F19,
        F20 = SDL_SCANCODE_F20,
        F21 = SDL_SCANCODE_F21,
        F22 = SDL_SCANCODE_F22,
        F23 = SDL_SCANCODE_F23,
        KPZero = SDL_SCANCODE_KP_0,
        KPOne = SDL_SCANCODE_KP_1,
        KPTwo = SDL_SCANCODE_KP_2,
        KPThree = SDL_SCANCODE_KP_3,
        KPFour = SDL_SCANCODE_KP_4,
        KPFive = SDL_SCANCODE_KP_5,
        KPSix = SDL_SCANCODE_KP_6,
        KPSeven = SDL_SCANCODE_KP_7,
        KPEight = SDL_SCANCODE_KP_8,
        KPNine = SDL_SCANCODE_KP_9,
        KPDot = SDL_SCANCODE_KP_PERIOD,
        KPSlash = SDL_SCANCODE_KP_DIVIDE,
        KPStar = SDL_SCANCODE_KP_MULTIPLY,
        KPDash = SDL_SCANCODE_KP_MINUS,
        KPPlus = SDL_SCANCODE_KP_PLUS,
        KPEnter = SDL_SCANCODE_KP_ENTER,
        KPEquals = SDL_SCANCODE_KP_EQUALS,
        LShift = SDL_SCANCODE_LSHIFT,
        LControl = SDL_SCANCODE_LCTRL,
        LAlt = SDL_SCANCODE_LALT,
        LWindowsKey = SDL_SCANCODE_LGUI,
        RShift = SDL_SCANCODE_RSHIFT,
        RControl = SDL_SCANCODE_RCTRL,
        RAlt = SDL_SCANCODE_LALT,
        RWindowsKey = SDL_SCANCODE_RGUI,

        Size = SDL_SCANCODE_COUNT
    };

    enum class KeyState : u8
    {
        Up = 0,
        Down = 1 << 0,
        StateChanged = 1 << 1,
        Pressed = Down | StateChanged,
        Released = Up | StateChanged,
        None = 0
    };

    ENUM_OPERATOR(KeyState, |);
    ENUM_OPERATOR(KeyState, &);
    ENUM_OPERATOR(KeyState, ^);
    ENUM_OPERATOR(KeyState, <<);
    ENUM_OPERATOR_UNARY(KeyState, ~);

    inline bool IsKeyDown(KeyState state) { return (bool) (state & KeyState::Down); }

    class KeyboardState
    {
    public:
        KeyboardState() = default;

        ALWAYS_INLINE KeyState GetKey(Key key) const
        {
            if(key == Key::None) return DEFAULT;
            return _keys[(u16) key];
        }

        void ProcessKey(const SDL_KeyboardEvent&);
        void ClearKeyStates();

        GET_SET(bool, IsFocused, _focused);

    private:
        KeyState _keys[(u16) Key::Size] = DEFAULT;
        bool _focused = false;
    };

    struct Shortcut
    {
    public:
        bool IsPressed(const KeyboardState&) const;

        Key First = Key::None;
        Key Second = Key::None;
        Key Third = Key::None;
    };
}