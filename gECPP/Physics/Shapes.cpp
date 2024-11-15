//
// Created by scion on 11/9/2024.
//

#include "Shapes.h"

namespace Jolt
{
    SphereShape::SphereShape(gE::Window*, SUPER&& INTERNAL_SETTINGS) :
        Jolt::ConvexShape(*this, *_shape)
    {
        SAFE_CONSTRUCT_NAMESPACE(_shape, ManagedPX<px::SphereShape>, gE, INTERNAL_SETTINGS.Radius);
    }

    BoxShape::BoxShape(gE::Window*, SUPER&& INTERNAL_SETTINGS) :
        Jolt::ConvexShape(*this, *_shape)
    {
        SAFE_CONSTRUCT_NAMESPACE(_shape, ManagedPX<px::BoxShape>, gE, gE::ToPX(INTERNAL_SETTINGS.Extents));
    }
}