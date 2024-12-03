//
// Created by scion on 11/9/2024.
//

#include "Shapes.h"

#include "Engine/Utility/AssetManager.h"
#include "gEModel/Engine/Utility/Vertex.h"

namespace Physics
{
    BakeConvexShapeResult ConvexMeshShape::Bake()
    {
        // I hate the unnecessary memory copies, but oh wells
    	BakedSettings = gE::ptr_create<BakedConvexMeshShape>();

        px::Shape::ShapeResult result;
        px::ConvexHullShapeSettings settings = DEFAULT;

        settings.mPoints = gE::ToPX<glm::vec3, px::Vec3, gE::ToPX>(Points);

        px::ConvexHullShape shape(settings, result);
        px::BakedConvexHullShapeSettings bakedPXSettings = DEFAULT;
        shape.GetBakedHullShapeSettings(bakedPXSettings);

        BakedSettings->CenterOfMass = gE::ToGLM(bakedPXSettings.mCenterOfMass);
        BakedSettings->Inertia = gE::ToGLM(bakedPXSettings.mInertia);
        BakedSettings->Bounds = gE::ToGE(bakedPXSettings.mLocalBounds);
        BakedSettings->Points = gE::ToGE<px::ConvexHullPoint, ConvexMeshPoint, ToGE>(bakedPXSettings.mPoints);
        BakedSettings->Faces = gE::ToGE<px::ConvexHullFace, ConvexMeshFace>(bakedPXSettings.mFaces);
        BakedSettings->VertexIDs = gE::ToGE<px::uint8, u8>(bakedPXSettings.mVertexIdx);
        BakedSettings->ConvexRadius = bakedPXSettings.mConvexRadius;
        BakedSettings->Volume = bakedPXSettings.mVolume;
        BakedSettings->InnerRadius = bakedPXSettings.mInnerRadius;

        return BakeConvexShapeResult::Success;
    }

    void BakedConvexMeshShape::Free()
    {
        Points.Free();
        Faces.Free();
        VertexIDs.Free();
    }

    bool BakedConvexMeshShape::IsFree() const
    {
        return Points.IsFree() && Faces.IsFree() && VertexIDs.IsFree();
    }

    ConvexMeshPoint ToGE(const px::ConvexHullPoint& o)
    {
        ConvexMeshPoint point;

        point.Position = gE::ToGLM(o.mPosition);
        point.FaceCount = o.mNumFaces;
        memcpy(point.Faces, o.mFaces, sizeof(ConvexMeshPoint::Faces));

        return point;
    }

    px::ConvexHullPoint ToPX(const ConvexMeshPoint& o)
    {
        px::ConvexHullPoint point;

        point.mPosition = gE::ToPX(o.Position);
        point.mNumFaces = o.FaceCount;
        memcpy(point.mFaces, o.Faces, sizeof(ConvexMeshPoint::Faces));

        return point;
    }
}

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
        const float height = Height * 0.5f - Radius;

        SAFE_CONSTRUCT_NAMESPACE(_shape, gE, ManagedPX<px::CapsuleShape>, height, Radius);
    }

    API_SERIALIZABLE_IMPL(ConvexMeshShape), Jolt::ConvexShape(*this, _shape.To<px::ConvexShape>())
    {
        GE_ASSERT((bool) BakedSettings, "MESH NOT BAKED BEFORE CREATION!");

        px::BakedConvexHullShapeSettings settings;

        SAFE_CONSTRUCT_NAMESPACE(_shape, gE, ManagedPX<px::ConvexHullShape>, move(settings));

        Free();
    }
}
