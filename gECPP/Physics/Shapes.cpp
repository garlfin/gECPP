//
// Created by scion on 11/9/2024.
//

#include "Shapes.h"

namespace Jolt
{
    API_SERIALIZABLE_IMPL(SphereShape), Jolt::ConvexShape(*this, _shape.To<px::ConvexShape>())
    {
        SAFE_CONSTRUCT_NAMESPACE(_shape, gE, ManagedPX<px::SphereShape>, Radius);
    }

    API_SERIALIZABLE_IMPL(BoxShape), Jolt::ConvexShape(*this, _shape.To<px::ConvexShape>())
    {
        SAFE_CONSTRUCT_NAMESPACE(_shape, gE, ManagedPX<px::BoxShape>, gE::ToPX(Extents));
    }

    API_SERIALIZABLE_IMPL(CapsuleShape), Jolt::ConvexShape(*this, _shape.To<px::ConvexShape>())
    {
        SAFE_CONSTRUCT_NAMESPACE(_shape, gE, ManagedPX<px::CapsuleShape>, Height / 2.f, Radius);
    }
}