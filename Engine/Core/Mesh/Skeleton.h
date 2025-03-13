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

    template<class BONE_T, class SKELETON_T>
    struct BoneReference : public Serializable<SKELETON_T&>
    {
        SERIALIZABLE_PROTO("BREF", 1, BoneReference, Serializable<SKELETON_T&>);

    public:
        BONE_T* Resolve(const SKELETON_T& skel);
        void Set(const SKELETON_T& skel, BONE_T& bone) const;

        BONE_T* Pointer = DEFAULT;
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
        BoneReference<Bone, Skeleton> Parent = DEFAULT;

        TransformData Transform = DEFAULT;
        glm::mat4 InverseBindMatrix = DEFAULT;
    };

    struct Skeleton final : public Asset
    {
        SERIALIZABLE_PROTO("SKEL", 1, Skeleton, Asset);
        REFLECTABLE_PROTO(Skeleton, Asset, "gE::BoneReference");

    public:
        Bone* FindBone(const std::string&, u8 suggestedLocation = -1);
        NODISCARD Bone* FindBone(const std::string&, u8 suggestedLocation = -1) const;
        NODISCARD ALWAYS_INLINE u8 GetIndex(const Bone& bone) const { return &bone - Bones.Data(); }

        NODISCARD inline bool IsFree() const override { return Bones.IsFree(); }
        inline void Free() override { Bones.Free(); }

        std::string Name = DEFAULT;
        Array<Bone> Bones = DEFAULT;
    };

    struct AnimatedBone : public Serializable<Animation&>
    {
        SERIALIZABLE_PROTO("ABNE", 1, AnimatedBone, Serializable);

    public:
        bool Retarget(const Skeleton&);
        NODISCARD inline bool IsFree() const { return Frames.IsFree(); }
        inline void Free() { Frames.Free(); }

        BoneReference<Bone, Skeleton> Target = DEFAULT;
        BoneReference<AnimatedBone, Animation> Parent = DEFAULT;
        Array<Frame> Frames = DEFAULT;
    };

    struct Animation : public Asset
    {
        SERIALIZABLE_PROTO("ANIM", 1, Animation, Asset);

    public:
        NODISCARD AnimatedBone* FindBone(const std::string&, u8 suggestedLocation = -1) const;
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
        Location = Read<u16>(in);
        Name = Read<std::string>(in);
        Resolve(settings);
    }

    template <class T, class SKELETON_T>
    void BoneReference<T, SKELETON_T>::ISerialize(ostream& out) const
    {
        Write(out, Location);
        Write(out, Name);
    }

    template <class BONE_T, class SKELETON_T>
    BONE_T* BoneReference<BONE_T, SKELETON_T>::Resolve(const SKELETON_T& skel)
    {
        if(!Pointer && !Name.empty())
        {
            Pointer = skel.FindBone(Name, Location);
            Location = Pointer - skel.Bones.begin();
        }
        return Pointer;
    }

    template <class BONE_T, class SKELETON_T>
    void BoneReference<BONE_T, SKELETON_T>::Set(const SKELETON_T& skel, BONE_T& bone) const
    {
        Pointer = &bone;
        Name = bone.Name;
        Location = skel.GetIndex(bone);
    }
}
