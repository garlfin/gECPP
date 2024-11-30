//
// Created by scion on 11/30/2024.
//

#pragma once

#include <Engine/Math/Math.h>
#include <Engine/Utility/Macro.h>

struct GLFWwindow;

namespace gE
{
    enum class Key : u16
    {
        None = 0,
        Space = 32,
        Apostraphe = 39,
        Comma = 44,
        Mines = 45,
        Period = 46,
        Slash = 47,
        Zero = 48,
        One = 49,
        Two = 50,
        Three = 51,
        Four = 52,
        Five = 53,
        Six = 54,
        Seven = 55,
        Eight = 56,
        Nine = 57,
        Semicolon = 59,
        Equals = 61,
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LeftBracket = 91,
        BackSlash = 92,
        RightBracket = 93,
        BackTick = 96,
        Escape = 256,
        Enter = 257,
        Tab = 258,
        BackSpace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        Print = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,
        KPZero = 320,
        KPOne = 321,
        KPTwo = 322,
        KPThree = 323,
        KPFour = 324,
        KPFive = 325,
        KPSix = 326,
        KPSeven = 327,
        KPEight = 328,
        KPNine = 329,
        KPDot = 330,
        KPSlash = 331,
        KPStar = 332,
        KPDash = 333,
        KPPlus = 334,
        KPEnter = 335,
        KPEquals = 336,
        LShift = 340,
        LControl = 341,
        LAlt = 342,
        LWindowsKey = 343,
        RShift = 344,
        RControl = 345,
        RAlt = 346,
        RWindowsKey = 347,
        Last = RWindowsKey,
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

    inline bool IsPressed(KeyState state) { return (bool) (state & KeyState::Down); }

    class KeyboardState
    {
    public:
        KeyboardState() = default;

        ALWAYS_INLINE KeyState GetKey(Key key) const
        {
            if(key == Key::None) return DEFAULT;
            return _keys[(u16) key];
        }

        void Update(GLFWwindow*);

    private:
        KeyState _keys[(u16) Key::Last + 1] = DEFAULT;
    };
}