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
        if(_vao != b._vao) return _vao < b._vao;
        if(GetShader() != b.GetShader()) return GetShader() < b.GetShader();
        if(_submeshIndex != b._submeshIndex) return _submeshIndex < b._submeshIndex;
        if(_lod != b._lod) return _lod < b._lod;
        if(this != &b) return this < &b;
        return false;
    }

    inline void DrawCallManager::Register(const DrawCall* draw)
    {
        GE_ASSERT(draw);
        _draws.insert(draw);
    }

    inline void DrawCallManager::Remove(const DrawCall* draw)
    {
        _draws.erase(draw);
    }
}
