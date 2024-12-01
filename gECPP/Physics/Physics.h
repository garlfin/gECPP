//
// Created by scion on 11/7/2024.
//

#pragma once

#include <gECPP/Engine/Utility/Macro.h>
#include <gECPP/Engine/Math/Math.h>

#include <Vendor/Jolt/Jolt.h>
#include <Vendor/Jolt/Core/JobSystemThreadPool.h>
#include <Vendor/Jolt/Physics/PhysicsSystem.h>
#include <Vendor/Jolt/Physics/Collision/ObjectLayer.h>
#include <Engine/Entity/Layer.h>

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
    struct ManagedPX
    {
    public:
        template<typename... ARGS>
        explicit ManagedPX(ARGS&&... args) : _t(new T(std::forward<ARGS>(args)...))
        {
            _t->SetEmbedded();
        }

        ManagedPX() = default;

        DELETE_OPERATOR_COPY(ManagedPX);
        OPERATOR_MOVE_NOSUPER(ManagedPX,
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

        ~ManagedPX()
        {
            if(!_t) return;

            LOG("INFO: DELETED PXOBJECT\n\tCOUNT: " << _t->GetRefCount() << "\n\tFUNCTION: " << PRETTY_FUNCTION);

            delete _t;
            _t = nullptr;
        }

    private:
        T* _t = DEFAULT;
    };

    inline px::Vec3 ToPX(const glm::vec3& o) { return { o.x, o.y, o.z }; }
    inline glm::vec3 ToGLM(const px::Vec3& o) { return { o.GetX(), o.GetY(), o.GetZ() }; }
    inline px::Quat ToPX(const glm::quat& o) { return { o.x, o.y, o.z, o.w }; }
    inline glm::quat ToGLM(const px::Quat& o) { return { o.GetW(), o.GetX(), o.GetY(), o.GetZ() }; }
}

#include "Physics.inl"
