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
        ReadSerializable(in, Parent, &s);
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

    Bone* Skeleton::FindBone(std::string_view name, u16 suggestedLocation) const
    {
        if(suggestedLocation != -1 && Bones[suggestedLocation].Name == name)
            return &Bones[suggestedLocation];

        for (u8 i = 0; i < Bones.Count(); ++i)
            if(Bones[i].Name == name) return &Bones[i];
        return nullptr;
    }

    REFLECTABLE_FACTORY_IMPL(Skeleton, Skeleton);
    REFLECTABLE_ONGUI_IMPL(Skeleton,
        DrawField(Field{ "Name" }, Name, depth);
        DrawField(ArrayField<Field>{ "Bones", "", ArrayViewMode::Elements }, Bones, depth);
    )

    bool AnimationChannel::Retarget(const Skeleton* skel)
    {
        return Target.Resolve(skel);
    }

    void AnimationChannel::IDeserialize(istream& in, SETTINGS_T s)
    {
        ReadSerializable(in, Target, s);
        ReadArray<u16>(in, Frames);
    }

    void AnimationChannel::ISerialize(ostream& out) const
    {
        Write(out, Target);
        WriteArray<u16>(out, Frames);
    }

    void Animation::IDeserialize(istream& in, SETTINGS_T s)
    {
        Read(in, Name);
        Read(in, FPS);
        Read(in, FrameCount);
        ReadArraySerializable<u16>(in, Channels, nullptr);
    }

    void Animation::ISerialize(ostream& out) const
    {
        Write(out, Name);
        Write(out, FPS);
        Write(out, FrameCount);
        WriteArray<u16>(out, Channels);
    }

    AnimationChannel* Animation::FindChannel(std::string_view name, u16 suggestedLocation) const
    {
        if(suggestedLocation != -1 && Channels[suggestedLocation].Target.Name == name)
            return &Channels[suggestedLocation];

        for (u8 i = 0; i < Channels.Count(); ++i)
            if(Channels[i].Target.Name == name) return &Channels[i];
        return nullptr;
    }

    void Animation::Retarget(const Reference<gE::Skeleton>& skeleton)
    {
        if(!skeleton || Skeleton == skeleton) return;

        Skeleton = skeleton;
        for(u8 i = 0; i < Channels.Count(); i++)
            Channels[i].Retarget(Skeleton.GetPointer());
    }

    REFLECTABLE_ONGUI_IMPL(Animation, );
    REFLECTABLE_FACTORY_IMPL(Animation, Animation);
}
