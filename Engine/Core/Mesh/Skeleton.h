//
// Created by scion on 12/13/2024.
//

#pragma once

#include <Core/Pointer.h>
#include <Core/Math/Math.h>
#include <Core/Math/Transform.h>
#include <Core/Serializable/Asset.h>
#include <Core/Serializable/Serializable.h>

#define GE_MAX_BONES 256

namespace gE
{
    struct Skeleton;
    struct Animation;

    enum class AnimationInterpolationMode : u8
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
        AnimationInterpolationMode InterpolationMode = DEFAULT;
        TransformData Transform = DEFAULT;
    };

    template<class T, class SKELETON_T>
    struct BoneReference : public Serializable<SKELETON_T&>
    {
        SERIALIZABLE_PROTO("BREF", 1, BoneReference, Serializable<SKELETON_T&>);

    public:
        T* Parent = DEFAULT;
        u8 SuggestedLocation = -1;
    };

    struct Bone : public Serializable<Skeleton&>
    {
        SERIALIZABLE_PROTO("BONE", 1, Bone, Serializable);

    public:
        NODISCARD inline bool IsFree() const { return Name.empty(); }
        inline void Free() { Name.clear(); }

        std::string Name = DEFAULT;
        BoneReference<Bone, Skeleton> Parent = DEFAULT;

        TransformData Transform = DEFAULT;
        glm::mat4 InverseBindMatrix = DEFAULT;
    };

    struct Skeleton : public Asset
    {
        SERIALIZABLE_PROTO("SKEL", 1, Skeleton, Asset);

    public:
        Bone* FindBone(const std::string&, u8 suggestedLocation = -1);
        NODISCARD const Bone* FindBone(const std::string&, u8 suggestedLocation = -1) const;
        NODISCARD ALWAYS_INLINE u8 GetIndex(const Bone& bone) const { return &bone - Bones.Data(); }

        NODISCARD inline bool IsFree() const override { return Bones.IsFree(); }
        inline void Free() override { Bones.Free(); }

        Array<Bone> Bones = DEFAULT;
    };

    struct AnimatedBone : public Serializable<Animation&>
    {
        SERIALIZABLE_PROTO("ABNE", 1, AnimatedBone, Serializable);

    public:
        bool Retarget(Skeleton*);

        NODISCARD inline bool IsFree() const { return Name.empty() && Frames.IsFree(); }
        inline void Free() { Name.clear(); Frames.Free(); }

        std::string Name = DEFAULT;
        Bone* Target = DEFAULT;
        BoneReference<AnimatedBone, Animation> Parent = DEFAULT;

        Array<Frame> Frames = DEFAULT;
    };

    struct Animation : public Asset
    {
        SERIALIZABLE_PROTO("ANIM", 1, Animation, Asset);

    public:
        AnimatedBone* FindBone(const std::string&, u8 suggestedLocation = -1);
        NODISCARD const AnimatedBone* FindBone(const std::string&, u8 suggestedLocation = -1) const;
        NODISCARD ALWAYS_INLINE u8 GetIndex(const AnimatedBone& bone) const { return &bone - Bones.Data(); }

        NODISCARD inline bool IsFree() const override { return Skeleton.IsFree() && Bones.IsFree(); }
        inline void Free() override { Name.clear(); Skeleton.Free(); Bones.Free(); }

        void Retarget(const Reference<Skeleton>&);

        std::string Name = DEFAULT;

        u16 FPS = DEFAULT;
        u16 FrameCount = DEFAULT;

        Reference<Skeleton> Skeleton = DEFAULT;
        Array<AnimatedBone> Bones = DEFAULT;
    };

    template <class T, class SKELETON_T>
    void BoneReference<T, SKELETON_T>::IDeserialize(istream& in, SETTINGS_T settings)
    {
        Read(in, SuggestedLocation);

        const std::string parentName = Read<std::string>(in);
        Parent = settings.FindBone(parentName);
    }

    template <class T, class SKELETON_T>
    void BoneReference<T, SKELETON_T>::ISerialize(ostream& out) const
    {
        Write(out, SuggestedLocation);

        if(Parent)
            Write(out, Parent->Name);
        else
            Write(out, std::string());
    }
}
