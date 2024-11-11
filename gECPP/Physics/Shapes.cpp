//
// Created by scion on 11/9/2024.
//

#include "Shapes.h"

namespace Jolt
{
    inline SphereShape::SphereShape(gE::Window* window, SUPER&& INTERNAL_SETTINGS)
    {
        SAFE_CONSTRUCT_NAMESPACE(_shape, ManagedPX<px::SphereShape>, gE, INTERNAL_SETTINGS.Radius);
    }

    inline BoxShape::BoxShape(gE::Window* window, SUPER&& INTERNAL_SETTINGS)
    {
        SAFE_CONSTRUCT_NAMESPACE(_shape, ManagedPX<px::BoxShape>, gE, INTERNAL_SETTINGS.Extents);
    }
}