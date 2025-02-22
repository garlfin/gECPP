//
// Created by scion on 10/1/2024.
//

#pragma once

#include "DrawCall.h"

namespace gE
{
    inline bool DrawCallCompare::operator()(const DrawCall* a, const DrawCall* b) const
    {
        return *a < *b;
    }

    inline bool DrawCall::operator<(const DrawCall& b) const
    {
        return _vao < b._vao &&
               _material < b._material &&
               _materialIndex < b._materialIndex &&
               _lod < b._lod;
    }

    inline DrawCallManager::SET_T::iterator DrawCallManager::Register(const DrawCall* draw)
    {
        GE_ASSERT(draw);
        return _draws.insert(draw).first;
    }

    inline void DrawCallManager::Remove(const DrawCall* draw)
    {
        _draws.erase(draw->GetIterator());
    }
}
