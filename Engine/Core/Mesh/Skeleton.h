//
// Created by scion on 12/13/2024.
//

#pragma once

#include <Core/Pointer.h>
#include <Core/Math/Math.h>
#include <Core/Math/Transform.h>
#include <Core/Serializable/Asset.h>
#include <Core/Serializable/Serializable.h>

#define GE_MAX_BONES (UINT16_MAX - 1)

namespace gE
{
    struct Skeleton;
    struct Animation;
    struct Bone;
}

template struct TypeSystem<gE::Skeleton&>;

template struct TypeSystem<gE::Animation&>;

namespace gE
{
    enum class FrameInterpolationMode : u8
    {
        None,
        Linear,
        Ease,
        EaseIn,
        EaseOut
    };

    struct Frame
    {
    public:
        u16 FrameID = DEFAULT;
        FrameInterpolationMode InterpolationMode = DEFAULT;
        TransformData Transform = DEFAULT;
    };

    struct BoneReference : public Serializable<Skeleton*>
    {
        SERIALIZABLE_PROTO("BREF", 1, BoneReference, Serializable);

    public:
        Bone* Resolve(const Skeleton* skel);
        void Set(const Skeleton* skel, Bone& bone);
        void Set(const Skeleton& skel, std::string_view name);
        void Optimize(const Skeleton* skel);

        Bone* Pointer = DEFAULT;
        u16 Location = -1;
        std::string Name = DEFAULT;
    };

    struct Bone : public Serializable<Skeleton&>
    {
        SERIALIZABLE_PROTO("BONE", 1, Bone, Serializable);
        REFLECTABLE_PROTO(Bone, Serializable, "gE::Bone");

    public:
        NODISCARD inline bool IsFree() const { return Name.empty(); }
        inline void Free() { Name.clear(); }

        std::string Name = DEFAULT;
        BoneReference Parent = DEFAULT;

        TransformData Transform = DEFAULT;
        glm::mat4 InverseBindMatrix = DEFAULT;
    };

    struct Skeleton final : public Asset
    {
        SERIALIZABLE_PROTO("SKEL", 1, Skeleton, Asset);
        REFLECTABLE_PROTO(Skeleton, Asset, "gE::BoneReference");

    public:
        NODISCARD Bone* FindBone(std::string_view name, u16 suggestedLocation = -1) const;
        NODISCARD ALWAYS_INLINE u16 GetIndex(const Bone& bone) const { return &bone - Bones.Data(); }

        NODISCARD inline bool IsFree() const override { return Bones.IsFree(); }
        inline void Free() override { Bones.Free(); }

        std::string Name = DEFAULT;
        Array<Bone> Bones = DEFAULT;
    };

    struct AnimationChannel : public Serializable<Skeleton*>
    {
        SERIALIZABLE_PROTO("ABNE", 1, AnimationChannel, Serializable);

    public:
        bool Retarget(const Skeleton*);
        NODISCARD inline bool IsFree() const { return Frames.IsFree(); }
        inline void Free() { Frames.Free(); }

        BoneReference Target = DEFAULT;
        Array<Frame> Frames = DEFAULT;
    };

    struct Animation : public Asset
    {
        SERIALIZABLE_PROTO("ANIM", 1, Animation, Asset);
        REFLECTABLE_PROTO(Animation, Asset, "gE::Animation");

    public:
        NODISCARD AnimationChannel* FindChannel(std::string_view name, u16 suggestedLocation = -1) const;

        NODISCARD inline bool IsFree() const override { return Skeleton.IsFree() && Channels.IsFree(); }
        inline void Free() override { Name.clear(); Skeleton.Free(); Channels.Free(); }

        void Retarget(const Reference<Skeleton>&);

        std::string Name = DEFAULT;

        u16 FPS = DEFAULT;
        u16 FrameCount = DEFAULT;

        Reference<Skeleton> Skeleton = DEFAULT;
        Array<AnimationChannel> Channels = DEFAULT;
    };
}

#include "Skeleton.inl"
