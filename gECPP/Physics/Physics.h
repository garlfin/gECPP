//
// Created by scion on 11/7/2024.
//

#pragma once

#include <gECPP/Engine/Utility/Macro.h>
#include <gECPP/Engine/Math/Math.h>

#include <Vendor/Jolt/Jolt.h>
#include <Vendor/Jolt/Physics/PhysicsSystem.h>
#include <Vendor/Jolt/Physics/Collision/ObjectLayer.h>
#include <Engine/Entity/Layer.h>
#include <Engine/Utility/Array.h>

#include "Engine/Math/Collision/AABB.h"

#define GE_PX_ALLOCATION 10485760
#define GE_PX_MAX_BODIES 1024
#define GE_PX_MAX_CONSTRAINTS (GE_PX_MAX_BODIES * 4)
#define GE_PX_MAX_BODY_PAIRS (GE_PX_MAX_BODIES * 4)
#define GE_PX_MIN_TICKRATE 60
#define GE_PX_MAX_STEPS 4

namespace px = JPH;

namespace gE
{
    class CollisionFilter final : public px::ObjectLayerPairFilter
    {
    public:
        NODISCARD inline bool ShouldCollide(px::ObjectLayer a, px::ObjectLayer b) const override;
        NODISCARD static inline bool ShouldCollideStatic(px::ObjectLayer a, px::ObjectLayer b);
    };

    class BroadPhaseFilter final : public px::ObjectVsBroadPhaseLayerFilter
    {
    public:
        NODISCARD inline bool ShouldCollide(px::ObjectLayer a, px::BroadPhaseLayer b) const override;
        NODISCARD static inline bool ShouldCollideStatic(px::ObjectLayer a, px::BroadPhaseLayer b);
    };

    // Unary CollisionFilter
    class UCollisionFilter final : public px::ObjectLayerFilter
    {
    public:
        UCollisionFilter() = default;
        explicit UCollisionFilter(LayerMask layer) : _layer((px::ObjectLayer) layer) {};

        bool ShouldCollide(JPH::ObjectLayer inLayer) const override;

    private:
        px::ObjectLayer _layer = DEFAULT;
    };

    // Unary BroadPhaseFilter
    class UBroadPhaseFilter final : public px::BroadPhaseLayerFilter
    {
    public:
        UBroadPhaseFilter() = default;
        explicit UBroadPhaseFilter(LayerMask layer) : _layer((px::ObjectLayer) layer) {};

        bool ShouldCollide(JPH::BroadPhaseLayer inLayer) const override;

    private:
        px::ObjectLayer _layer = DEFAULT;
    };

    class BroadPhase final : public px::BroadPhaseLayerInterface
    {
    public:
        BroadPhase();

        NODISCARD inline px::uint GetNumBroadPhaseLayers() const override;
        NODISCARD inline px::BroadPhaseLayer GetBroadPhaseLayer(px::ObjectLayer inLayer) const override;

    private:
        px::BroadPhaseLayer _layers[2] = DEFAULT;
    };

    GLOBAL px::ShapeFilter DefaultShapeFilter = DEFAULT;
    GLOBAL px::BodyFilter DefaultBodyFilter = DEFAULT;

    template<class T>
    struct ManagedPX : public Asset
    {
    public:
        template<typename... ARGS>
        explicit ManagedPX(ARGS&&... args) : _t(new T(std::forward<ARGS>(args)...))
        {
            _t->SetEmbedded();
        }

        explicit ManagedPX(T* t) : _t(t)
        {
            if(!t) return;

            GE_ASSERT(t->GetRefCount() < 0x0ebedded, "PXOBJECT ALREADY EMBEDDED");
            t->SetEmbedded();
        }

        ManagedPX() = default;

        DELETE_OPERATOR_COPY(ManagedPX);
        OPERATOR_MOVE_NOSUPER(ManagedPX, Free,
            if(_t) LOG("INFO: MOVED PXOBJECT\n\tCOUNT: " << _t->GetRefCount() << "\n\tFUNCTION: " << PRETTY_FUNCTION);
            _t = o._t;
            o._t = nullptr;
        );

        GET(T&,, _t);
        GET(T*, Pointer, _t);

        ALWAYS_INLINE T* operator->() { return _t; }
        ALWAYS_INLINE const T* operator->() const { return _t; }
        ALWAYS_INLINE T& operator*() { return *_t; }
        ALWAYS_INLINE const T& operator*() const { return *_t; }

        template<class O> requires std::is_base_of_v<O, T>
        ManagedPX<O>& To() { return (ManagedPX<O>&) *this; }

        template<class O> requires std::is_base_of_v<O, T>
        const ManagedPX<O>& To() const { return (ManagedPX<O>&) *this; }

        void Free() override
        {
            if(!_t) return;

            LOG("INFO: DELETED PXOBJECT\n\tFUNCTION: " << PRETTY_FUNCTION);

            delete _t;
            _t = nullptr;
        }

        NODISCARD bool IsFree() const override { return !_t; }

        inline ~ManagedPX() override { ManagedPX::Free(); }

    private:
        T* _t = DEFAULT;
    };
}

namespace Physics
{
    template<typename IN_T, typename OUT_T>
    using PXCastFunction = OUT_T(*)(const IN_T& from);

    template<typename IN_T, typename OUT_T>
    OUT_T PXBitCast(const IN_T& in)
    {
        return *(OUT_T*) &in;
    }

    NODISCARD inline px::Vec3 ToPX(const glm::vec3& o) { return { o.x, o.y, o.z }; }
    NODISCARD inline glm::vec3 ToGLM(const px::Vec3& o) { return { o.GetX(), o.GetY(), o.GetZ() }; }
    NODISCARD inline px::Quat ToPX(const glm::quat& o) { return { o.x, o.y, o.z, o.w }; }
    NODISCARD inline glm::quat ToGLM(const px::Quat& o) { return { o.GetW(), o.GetX(), o.GetY(), o.GetZ() }; }
    NODISCARD inline px::Mat44 ToPX(const glm::mat4& o) { return PXBitCast<glm::mat4, px::Mat44>(o); }
    NODISCARD inline glm::mat4 ToGLM(const px::Mat44& o) { return PXBitCast<px::Mat44, glm::mat4>(o); }
    NODISCARD inline gE::AABB<Dimension::D3D> ToGE(const px::AABox& o) { return { ToGLM(o.mMin), ToGLM(o.mMax) }; }
    NODISCARD inline px::AABox ToPX(const gE::AABB<Dimension::D3D>& o) { return { ToPX(o.Min), ToPX(o.Max) }; }

    template<class IN_T, class OUT_T, PXCastFunction<IN_T, OUT_T> CAST_FUNC = PXBitCast<IN_T, OUT_T>>
    px::Array<OUT_T> ToPX(const Array<IN_T>& array)
    {
        if constexpr(CAST_FUNC == PXBitCast<IN_T, OUT_T>)
        {
            static_assert(std::is_trivially_copyable_v<IN_T>);
            static_assert(std::is_trivially_copyable_v<OUT_T>);
            static_assert(sizeof(IN_T) == sizeof(OUT_T));

            return px::Array<OUT_T>((OUT_T*) array.Data(), (OUT_T*) array.Data() + array.Count());
        }

        px::Array<OUT_T> arr(array.Count());

        for(size_t i = 0; i < array.Count(); i++)
            arr[i] = CAST_FUNC(array[i]);

        return arr;
    }

    template<class IN_T, class OUT_T, PXCastFunction<IN_T, OUT_T> CAST_FUNC = PXBitCast<IN_T, OUT_T>>
    Array<OUT_T> ToGE(const px::Array<IN_T>& array)
    {
        if constexpr(CAST_FUNC == PXBitCast<IN_T, OUT_T>)
        {
            static_assert(std::is_trivially_copyable_v<IN_T>);
            static_assert(std::is_trivially_copyable_v<OUT_T>);
            static_assert(sizeof(IN_T) == sizeof(OUT_T));

            return Array<OUT_T>((OUT_T*) array.data(), array.size());
        }

        Array<OUT_T> arr(array.size());

        for(size_t i = 0; i < arr.Count(); i++)
            arr[i] = CAST_FUNC(array[i]);

        return arr;
    }
}

#include "Physics.inl"
