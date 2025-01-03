//
// Created by scion on 12/13/2024.
//

#include "Skeleton.h"

namespace gE
{
    void Bone::ISerialize(istream& in, SETTINGS_T s)
    {
        Read(in, Name);
        ReadSerializable(in, Parent, s);
        Read(in, Transform);
        InverseBindMatrix = inverse(Transform.ToMat4());
    }

    void Bone::IDeserialize(ostream& out) const
    {
        Write(out, Name);
        Write(out, Parent);
        Write(out, Transform);
    }

    void AnimatedBone::ISerialize(istream& in, SETTINGS_T s)
    {
        Read(in, Name);
        ReadSerializable(in, Parent, s);
        ReadArray<u16>(in, Frames);
    }

    void AnimatedBone::IDeserialize(ostream& out) const
    {
        Write(out, Name);
        Write(out, Parent);
        WriteArray<u16>(out, Frames);
    }

    bool AnimatedBone::Retarget(Skeleton* skeleton)
    {
        GE_ASSERT(skeleton, "NO SKELETON PROVIDED!");

        if(Target) Name = Target->Name;
        Target = skeleton->FindBone(Name);

        if(Target) Name.clear();
        return Target;
    }

    void Skeleton::ISerialize(istream& in, SETTINGS_T s)
    {
        ReadArraySerializable<u8>(in, Bones, *this);
    }

    void Skeleton::IDeserialize(ostream& out) const
    {
        WriteArray<u8>(out, Bones);
    }

    Bone* Skeleton::FindBone(const std::string& name, u8 suggestedLocation)
    {
        GE_ASSERT(Bones.Count() <= GE_MAX_BONES, "TOO MANY BONES!");

        if(suggestedLocation != -1 && Bones[suggestedLocation].Name == name)
            return &Bones[suggestedLocation];

        for (u8 i = 0; i < Bones.Count(); ++i)
            if(Bones[i].Name == name) return &Bones[i];
        return nullptr;
    }

    const Bone* Skeleton::FindBone(const std::string& name, u8 suggestedLocation) const
    {
        GE_ASSERT(Bones.Count() <= GE_MAX_BONES, "TOO MANY BONES!");

        if(suggestedLocation != -1 && Bones[suggestedLocation].Name == name)
            return &Bones[suggestedLocation];

        for (u8 i = 0; i < Bones.Count(); ++i)
            if(Bones[i].Name == name) return &Bones[i];
        return nullptr;
    }

    void Animation::ISerialize(istream& in, SETTINGS_T s)
    {
        Read(in, Name);
        Read(in, FPS);
        Read(in, FrameCount);
        ReadArraySerializable<u8>(in, Bones, *this);
    }

    void Animation::IDeserialize(ostream& out) const
    {
        Write(out, Name);
        Write(out, FPS);
        Write(out, FrameCount);
        WriteArray<u8>(out, Bones);
    }

    AnimatedBone* Animation::FindBone(const std::string& name, u8 suggestedLocation)
    {
        GE_ASSERT(Bones.Count() <= GE_MAX_BONES, "TOO MANY BONES!");

        if(suggestedLocation != -1 && Bones[suggestedLocation].Name == name)
            return &Bones[suggestedLocation];

        for (u8 i = 0; i < Bones.Count(); ++i)
            if(Bones[i].Name == name) return &Bones[i];
        return nullptr;
    }

    const AnimatedBone* Animation::FindBone(const std::string& name, u8 suggestedLocation) const
    {
        GE_ASSERT(Bones.Count() <= GE_MAX_BONES, "TOO MANY BONES!");

        if(suggestedLocation != -1 && Bones[suggestedLocation].Name == name)
            return &Bones[suggestedLocation];

        for (u8 i = 0; i < Bones.Count(); ++i)
            if(Bones[i].Name == name) return &Bones[i];
        return nullptr;
    }

    void Animation::Retarget(const Reference<gE::Skeleton>& skeleton)
    {
        GE_ASSERT((bool) skeleton, "NO SKELETON PROVIDED!");
        GE_ASSERT(Bones.Count() <= GE_MAX_BONES, "TOO MANY BONES!");

        if(Skeleton == skeleton) return;

        Skeleton = skeleton;
        for(u8 i = 0; i < Bones.Count(); i++)
            Bones[i].Retarget(Skeleton.GetPointer());
    }
}
