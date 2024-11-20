//
// Created by scion on 11/9/2024.
//

#include "Shapes.h"

namespace Jolt
{
    SphereShape::SphereShape(gE::Window*, SUPER&& INTERNAL_SETTINGS) :
        Jolt::ConvexShape(*this, *_shape)
    {
        SAFE_CONSTRUCT_NAMESPACE(_shape, gE, ManagedPX<px::SphereShape>, INTERNAL_SETTINGS.Radius);
    }

    BoxShape::BoxShape(gE::Window*, SUPER&& INTERNAL_SETTINGS) :
        Jolt::ConvexShape(*this, *_shape)
    {
        SAFE_CONSTRUCT_NAMESPACE(_shape, gE, ManagedPX<px::BoxShape>, gE::ToPX(INTERNAL_SETTINGS.Extents));
    }
}