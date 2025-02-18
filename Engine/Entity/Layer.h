//
// Created by scion on 10/30/2024.
//

#pragma once

#include <Core/Macro.h>
#include <Core/Math/Math.h>

namespace gE
{
    enum class LayerMask : u8
    {
        Static = 1,
        One = 1 << 1,
        Two = 1 << 2,
        Three = 1 << 3,
        Four = 1 << 4,
        Five = 1 << 5,
        Six = 1 << 6,
        Seven = 1 << 7,
        All = 0b11111110,
        None = 0,
        Dynamic = 0,
    };

    ENUM_OPERATOR(LayerMask, |);
    ENUM_OPERATOR(LayerMask, &);

    namespace Layers
    {
        CONSTEXPR_GLOBAL LayerMask StaticObjects = LayerMask::Static | LayerMask::All;
    }
}
