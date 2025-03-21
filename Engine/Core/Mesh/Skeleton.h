//
// Created by scion on 12/13/2024.
//

#pragma once

#include <Core/Pointer.h>
#include <Core/Math/Math.h>
#include <Core/Math/Transform.h>
#include <Core/Serializable/Asset.h>
#include <Core/Serializable/Serializable.h>

#include "Core/RelativePointer.h"

#define GE_MAX_BONES (UINT16_MAX - 1)

namespace gE
{
    struct Skeleton;
    struct Animation;
    struct AnimationChannel;
    struct Bone;
}

template struct TypeSystem<gE::Skeleton&>;
template struct TypeSystem<gE::Skeleton*>;
template struct TypeSystem<const gE::AnimationChannel&>;

namespace gE
{
    enum class FrameInterpolationMode : u8
    {
        Step,
        Linear,
        Ease,
        EaseIn,
        EaseOut
    };

    REFLECTABLE_ENUM(Normal, FrameInterpolationMode, 5,
        REFLECT_ENUM(FrameInterpolationMode, Step),
        REFLECT_ENUM(FrameInterpolationMode, Linear),
        REFLECT_ENUM(FrameInterpolationMode, Ease),
        REFLECT_ENUM(FrameInterpolationMode, EaseIn),
        REFLECT_ENUM(FrameInterpolationMode, EaseOut),
    );

    enum class ChannelType : u8
    {
        Location = 1,
        Rotation = 2,
        Scale = 3
    };

    REFLECTABLE_ENUM(Normal, ChannelType, 3,
        REFLECT_ENUM(ChannelType, Location),
        REFLECT_ENUM(ChannelType, Rotation),
        REFLECT_ENUM(ChannelType, Scale)
    );

    struct Frame : public Serializable<const AnimationChannel&>
    {
        SERIALIZABLE_PROTO_NOHEADER("gE::Frame", Frame, Serializable);
        REFLECTABLE_NAME_PROTO();

    public:
        float Time;
        std::variant<glm::vec3, glm::quat> Value;

#ifdef GE_ENABLE_IMGUI
        RelativePointer<const AnimationChannel> Channel;
#endif
    };

    struct BoneReference : public Serializable<Skeleton*>
    {
        SERIALIZABLE_PROTO("gE::BoneReference", "BREF", 1, BoneReference, Serializable);
        REFLECTABLE_NAME_PROTO();

    public:
        Bone* Resolve(const Skeleton* skeleton);
        void Set(const Skeleton* skel, Bone& bone);
        void Set(const Skeleton& skel, std::string_view name);
        void Optimize(const Skeleton* skel);

        RelativePointer<Bone> Pointer = DEFAULT;
        u16 Location = -1;
        std::string Name = DEFAULT;
    };

    struct Bone : public Serializable<Skeleton&>
    {
        SERIALIZABLE_PROTO("gE::Bone", "BONE", 1, Bone, Serializable);
        REFLECTABLE_NAME_PROTO();

    public:
        NODISCARD bool IsFree() const { return Name.empty(); }
        void Free() { Name.clear(); }

        std::string Name = DEFAULT;
        BoneReference Parent = DEFAULT;

        TransformData Transform = DEFAULT;
        glm::mat4 InverseBindMatrix = DEFAULT;
    };

    struct Skeleton final : public Asset
    {
        SERIALIZABLE_PROTO("gE::Skeleton", "SKEL", 1, Skeleton, Asset);
        REFLECTABLE_NAME_PROTO();

    public:
        NODISCARD Bone* FindBone(std::string_view name, u16 suggestedLocation = -1) const;
        NODISCARD ALWAYS_INLINE u16 GetIndex(const Bone& bone) const { return &bone - Bones.Data(); }

        NODISCARD bool IsFree() const override { return Bones.IsFree(); }
        void Free() override { Bones.Free(); }

        std::string Name = DEFAULT;
        Array<Bone> Bones = DEFAULT;
    };

    struct AnimationChannel final : public Serializable<Skeleton*>
    {
        SERIALIZABLE_PROTO("gE::AnimationChannel", "ABNE", 1, AnimationChannel, Serializable);
        REFLECTABLE_NAME_PROTO();

    public:
        NODISCARD inline const Frame* GetFrame(float time) const;
        bool Retarget(const Skeleton*);

        NODISCARD bool IsFree() const { return Frames.IsFree(); }
        void Free() { Frames.Free(); }

        BoneReference Target = DEFAULT;
        ChannelType Type = DEFAULT;
        Array<Frame> Frames = DEFAULT;
    };

    struct Animation : public Asset
    {
        SERIALIZABLE_PROTO("gE::Animation", "ANIM", 1, Animation, Asset);
        REFLECTABLE_NAME_PROTO();

    public:
        NODISCARD AnimationChannel* FindChannel(std::string_view name, u16 suggestedLocation = -1) const;

        GET_CONST(const Reference<Skeleton>&, Skeleton, _skeleton);
        void SetSkeleton(const Reference<Skeleton>&);

        NODISCARD inline bool IsFree() const override { return _skeleton.IsFree() && Channels.IsFree(); }
        inline void Free() override { Name.clear(); _skeleton.Free(); Channels.Free(); }

        std::string Name = DEFAULT;
        float Length = DEFAULT;

        Array<AnimationChannel> Channels = DEFAULT;

    private:
        Reference<Skeleton> _skeleton = DEFAULT;
    };
}

#include "Skeleton.inl"
