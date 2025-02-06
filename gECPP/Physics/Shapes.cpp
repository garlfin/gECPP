//
// Created by scion on 11/9/2024.
//

#include "Shapes.h"

#include <Utility/AssetManager.h>

namespace Physics
{
    BakeConvexShapeResult ConvexMeshShape::Bake()
    {
        // I hate the unnecessary memory copies, but oh wells
    	BakedSettings = gE::ptr_create<BakedConvexMeshShape>();

        px::ConvexHullShapeSettings settings = DEFAULT;
        settings.mPoints = ToPX<glm::vec3, px::Vec3, ToPX>(Points);

        px::BakedConvexHullShapeSettings::EResult result;
        px::BakedConvexHullShapeSettings bakedPXSettings(settings, result);

        BakedSettings.CenterOfMass = ToGLM(bakedPXSettings.mCenterOfMass);
        BakedSettings.Inertia = ToGLM(bakedPXSettings.mInertia);
        BakedSettings.Bounds = ToGE(bakedPXSettings.mLocalBounds);
        BakedSettings.Points = ToGE<px::ConvexHullPoint, ConvexMeshPoint, ToGE>(bakedPXSettings.mPoints);
        BakedSettings.Faces = ToGE<px::ConvexHullFace, ConvexMeshFace>(bakedPXSettings.mFaces);
        BakedSettings.VertexIDs = ToGE<px::uint8, u8>(bakedPXSettings.mVertexIdx);
        BakedSettings.ConvexRadius = bakedPXSettings.mConvexRadius;
        BakedSettings.Volume = bakedPXSettings.mVolume;
        BakedSettings.InnerRadius = bakedPXSettings.mInnerRadius;

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

        point.Position = ToGLM(o.mPosition);
        point.FaceCount = o.mNumFaces;
        memcpy(point.Faces, o.mFaces, sizeof(ConvexMeshPoint::Faces));

        return point;
    }

    px::ConvexHullPoint ToPX(const ConvexMeshPoint& o)
    {
        px::ConvexHullPoint point;

        point.mPosition = ToPX(o.Position);
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
        SAFE_CONSTRUCT_NAMESPACE(_shape, gE, ManagedPX<px::BoxShape>, Physics::ToPX(Extents));
    }

    API_SERIALIZABLE_IMPL(CapsuleShape), Jolt::ConvexShape(*this, _shape.To<px::ConvexShape>())
    {
        const float height = Height * 0.5f - Radius;

        SAFE_CONSTRUCT_NAMESPACE(_shape, gE, ManagedPX<px::CapsuleShape>, height, Radius);
    }

    API_SERIALIZABLE_IMPL(ConvexMeshShape), Jolt::ConvexShape(*this, _shape.To<px::ConvexShape>())
    {
        GE_ASSERTM(!BakedSettings.IsFree(), "MESH NOT BAKED BEFORE CREATION!");

        px::BakedConvexHullShapeSettings settings;
        settings.mCenterOfMass = Physics::ToPX(BakedSettings.CenterOfMass);
        settings.mInertia = Physics::ToPX(BakedSettings.Inertia);
        settings.mLocalBounds = Physics::ToPX(BakedSettings.Bounds);
        settings.mPoints = Physics::ToPX<Physics::ConvexMeshPoint, px::ConvexHullPoint, Physics::ToPX>(BakedSettings.Points);
        settings.mFaces = Physics::ToPX<Physics::ConvexMeshFace, px::ConvexHullFace>(BakedSettings.Faces);
        settings.mVertexIdx = Physics::ToPX<u8, px::uint8>(BakedSettings.VertexIDs);
        settings.mConvexRadius = BakedSettings.ConvexRadius;
        settings.mVolume = BakedSettings.Volume;
        settings.mInnerRadius = BakedSettings.InnerRadius;

        SAFE_CONSTRUCT_NAMESPACE(_shape, gE, ManagedPX<px::ConvexHullShape>, move(settings));

        Free();
    }
}
