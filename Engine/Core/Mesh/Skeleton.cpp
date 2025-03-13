//
// Created by scion on 12/13/2024.
//

#include "Skeleton.h"

#include "Core/GUI/Field.h"
#include "Core/GUI/Editor/Editor.h"

namespace gE
{
    void Bone::IDeserialize(istream& in, SETTINGS_T s)
    {
        Read(in, Name);
        ReadSerializable(in, Parent, s);
        Read(in, Transform);
        InverseBindMatrix = inverse(Transform.ToMat4());
    }

    void Bone::ISerialize(ostream& out) const
    {
        Write(out, Name);
        Write(out, Parent);
        Write(out, Transform);
    }

    REFLECTABLE_FACTORY_IMPL(Bone, Bone);
    REFLECTABLE_ONGUI_IMPL(Bone,
        DrawField(Field{ "Name" }, Name, depth);
        DrawField(Field{ "Parent" }, Parent.Pointer, depth);

        DrawField(ScalarField<float>{ "Position" }, Transform.Position, depth);
        DrawField(ScalarField<float>{ "Rotation" }, Transform.Rotation, depth);
        DrawField(ScalarField{ "Scale", "", FLT_EPSILON }, Transform.Scale, depth);
    );

    void AnimatedBone::IDeserialize(istream& in, Animation& animation)
    {
        ReadSerializable(in, Target, animation.Skeleton);
        ReadSerializable(in, Parent, animation);
        ReadArray<u16>(in, Frames);
    }

    void AnimatedBone::ISerialize(ostream& out) const
    {
        Write(out, Target);
        Write(out, Parent);
        WriteArray<u16>(out, Frames);
    }

    bool AnimatedBone::Retarget(const Skeleton& skeleton)
    {
        return Target.Resolve(skeleton);
    }

    void Skeleton::IDeserialize(istream& in, SETTINGS_T s)
    {
        Read(in, Name);
        ReadArraySerializable<u8>(in, Bones, *this);
    }

    void Skeleton::ISerialize(ostream& out) const
    {
        Write(out, Name);
        WriteArray<u8>(out, Bones);
    }

    REFLECTABLE_FACTORY_IMPL(Skeleton, Skeleton);
    REFLECTABLE_ONGUI_IMPL(Skeleton,
        DrawField(Field{ "Name" }, Name, depth);
        DrawField(ArrayField<Field>{ "Bones", "", ArrayViewMode::Elements }, Bones, depth);
    );

    Bone* Skeleton::FindBone(const std::string& name, u8 suggestedLocation)
    {
        GE_ASSERTM(Bones.Count() <= GE_MAX_BONES, "TOO MANY BONES!");

        if(suggestedLocation != -1 && Bones[suggestedLocation].Name == name)
            return &Bones[suggestedLocation];

        for (u8 i = 0; i < Bones.Count(); ++i)
            if(Bones[i].Name == name) return &Bones[i];
        return nullptr;
    }

    Bone* Skeleton::FindBone(const std::string& name, u8 suggestedLocation) const
    {
        GE_ASSERTM(Bones.Count() <= GE_MAX_BONES, "TOO MANY BONES!");

        if(suggestedLocation != -1 && Bones[suggestedLocation].Name == name)
            return &Bones[suggestedLocation];

        for (u8 i = 0; i < Bones.Count(); ++i)
            if(Bones[i].Name == name) return &Bones[i];
        return nullptr;
    }

    void Animation::IDeserialize(istream& in, SETTINGS_T s)
    {
        Read(in, Name);
        Read(in, FPS);
        Read(in, FrameCount);
        ReadArraySerializable<u16>(in, Bones, *this);
    }

    void Animation::ISerialize(ostream& out) const
    {
        Write(out, Name);
        Write(out, FPS);
        Write(out, FrameCount);
        WriteArray<u16>(out, Bones);
    }

    AnimatedBone* Animation::FindBone(const std::string& name, u8 suggestedLocation) const
    {
        GE_ASSERTM(Bones.Count() <= GE_MAX_BONES, "TOO MANY BONES!");

        if(suggestedLocation != -1 && Bones[suggestedLocation].Target.Name == name)
            return &Bones[suggestedLocation];

        for (u8 i = 0; i < Bones.Count(); ++i)
            if(Bones[i].Target.Name == name) return &Bones[i];
        return nullptr;
    }

    void Animation::Retarget(const Reference<gE::Skeleton>& skeleton)
    {
        GE_ASSERTM((bool) skeleton, "NO SKELETON PROVIDED!");
        GE_ASSERTM(Bones.Count() <= GE_MAX_BONES, "TOO MANY BONES!");

        if(Skeleton == skeleton) return;

        Skeleton = skeleton;
        for(u8 i = 0; i < Bones.Count(); i++)
            Bones[i].Retarget(*Skeleton);
    }
}
