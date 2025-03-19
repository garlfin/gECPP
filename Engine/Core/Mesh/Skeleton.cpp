//
// Created by scion on 12/13/2024.
//

#include "Skeleton.h"

#include "Core/GUI/Field.h"
#include "Core/GUI/Editor/Editor.h"

namespace gE
{
#ifdef GE_ENABLE_IMGUI
    void Frame::OnEditorGUI(u8 depth)
    {
        if(!Channel) return;

        size_t index = this - &Channel->Frames[0];

        const float minTime = index ? Channel->Frames[index - 1].Time + FLT_EPSILON : 0.f;
        const float maxTime = index + 1 == Channel->Frames.Count() ? FLT_MAX : Channel->Frames[index + 1].Time - FLT_EPSILON;

        DrawField<float>(ScalarField{ "Time", "", minTime, maxTime }, Time, depth);

        if(Channel->ChannelType == ChannelType::Location)
            DrawField(ScalarField<float>{ "Location" }, std::get<glm::vec3>(Value), depth);
        else if(Channel->ChannelType == ChannelType::Rotation)
            DrawField(ScalarField<float>{ "Rotation" }, std::get<glm::quat>(Value), depth);
        else if(Channel->ChannelType == ChannelType::Scale)
            DrawField(ScalarField{ "Scale", "", FLT_EPSILON }, std::get<glm::vec3>(Value), depth);
    }
#endif

    inline void BoneReference::IDeserialize(istream& in, Skeleton* skeleton)
    {
        Location = Read<u16>(in);
        Name = Read<std::string>(in);
        Resolve(skeleton);
    }

    inline void BoneReference::ISerialize(ostream& out) const
    {
        Write(out, Location);
        Write(out, Name);
    }

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
        DrawField(ScalarField<float>{ "Transform" }, Transform, depth);
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

    REFLECTABLE_ONGUI_IMPL(AnimationChannel,
        DrawField(Field{ "Target" }, Target, depth);
        DrawField(ArrayField<Field>{ "Frames" }, Frames, depth);
    );
    REFLECTABLE_FACTORY_IMPL(AnimationChannel, AnimationChannel);

    void Animation::IDeserialize(istream& in, SETTINGS_T s)
    {
        Read(in, Name);
        Read(in, Length);
        ReadArraySerializable<u16>(in, Channels, nullptr);
    }

    void Animation::ISerialize(ostream& out) const
    {
        Write(out, Name);
        Write(out, Length);
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

    REFLECTABLE_ONGUI_IMPL(Animation,
        DrawField(Field{ "Name" }, Name, depth);
        DrawField<const float>(ScalarField<float>{ "Length" }, Length, depth);
        DrawField(ArrayField<Field>{ "Channels" }, Channels, depth);
    );
    REFLECTABLE_FACTORY_IMPL(Animation, Animation);
}
