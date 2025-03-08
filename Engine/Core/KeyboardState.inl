//
// Created by scion on 3/7/2025.
//

#pragma once

#include "KeyboardState.h"

namespace gE
{
    inline bool Shortcut::operator==(const Shortcut& o)
    {
        bool same = true;

        if((bool) First || (bool) o.First) same &= First == o.First || First == o.Second;
        if((bool) Second || (bool) o.Second) same &= Second == o.First || Second == o.Second;
        same &= (bool) Third && Third == o.Third;

        return same;

    }

    ALWAYS_INLINE KeyState KeyboardState::GetKey(Key key) const
    {
        return (bool) key ? _keys[(u16) key] : KeyState::None;
    }

    ALWAYS_INLINE KeyState KeyboardState::GetKey(KeyModifier key) const
    {
        return GetKey((Key) key);
    }

    inline bool KeyboardState::GetShortcut(Shortcut shortcut) const
    {
        return shortcut == _shortcut;
    }
}
