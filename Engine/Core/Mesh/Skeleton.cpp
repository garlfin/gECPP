//
// Created by scion on 12/13/2024.
//

#include "Skeleton.h"

#include "Core/GUI/Field.h"
#include "Core/GUI/Editor/Editor.h"

namespace gE
{
    void Frame::IDeserialize(istream& in, const AnimationChannel& s)
    {
        Channel = s;

        Read(in, Time);
        if(Channel->Type == ChannelType::Location || Channel->Type == ChannelType::Rotation)
            Value = Read<glm::vec3>(in);
        else
            Value = Read<glm::quat>(in);
    }

    void Frame::ISerialize(ostream& out) const
    {
        Write(out, Time);
        if(std::holds_alternative<glm::vec3>(Value))
            Write(out, std::get<glm::vec3>(Value));
        else
            Write(out, std::get<glm::quat>(Value));
    }

    REFLECTABLE_ONGUI_IMPL(Frame,
        if(!Channel) return;

        size_t index = this - &Channel->Frames[0];

        const float minTime = index ? Channel->Frames[index - 1].Time + FLT_EPSILON : 0.f;
        const float maxTime = index + 1 == Channel->Frames.Count() ? FLT_MAX : Channel->Frames[index + 1].Time - FLT_EPSILON;

        DrawField<float>(ScalarField{ "Time", "", minTime, maxTime }, Time, depth);

        if(Channel->Type == ChannelType::Location)
            DrawField(ScalarField<float>{ "Location" }, std::get<glm::vec3>(Value), depth);
        else if(Channel->Type == ChannelType::Rotation)
            DrawField(ScalarField<float>{ "Rotation" }, std::get<glm::quat>(Value), depth);
        else if(Channel->Type == ChannelType::Scale)
            DrawField(ScalarField{ "Scale", "", FLT_EPSILON }, std::get<glm::vec3>(Value), depth);
    );
    REFLECTABLE_NAME_IMPL(Frame, return std::to_string(Time));
    REFLECTABLE_FACTORY_IMPL(Frame);

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

    REFLECTABLE_ONGUI_IMPL(BoneReference,
        if(Pointer)
            DrawField(Field{ "Target (Pointer)" }, Pointer.GetPointer(), depth);
        else
            DrawField<const std::string>(Field{ "Target (Name)" }, Name, depth);
    );
    REFLECTABLE_NAME_IMPL(BoneReference, return Name);
    REFLECTABLE_FACTORY_IMPL(BoneReference);

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

    REFLECTABLE_ONGUI_IMPL(Bone,
        DrawField(Field{ "Name" }, Name, depth);
        DrawField(Field{ "Parent" }, Parent.Pointer.GetPointer(), depth);
        DrawField(ScalarField<float>{ "Transform" }, Transform, depth);
    );
    REFLECTABLE_NAME_IMPL(Bone, return Name);
    REFLECTABLE_FACTORY_IMPL(Bone);

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
        if(suggestedLocation != (u16) -1 && Bones[suggestedLocation].Name == name)
            return &Bones[suggestedLocation];

        for (u8 i = 0; i < Bones.Count(); ++i)
            if(Bones[i].Name == name) return &Bones[i];
        return nullptr;
    }

    REFLECTABLE_ONGUI_IMPL(Skeleton,
        DrawField(Field{ "Name" }, Name, depth);
        DrawField(ArrayField<Field>{ "Bones", "", ArrayViewMode::Elements }, Bones, depth);
    )
    REFLECTABLE_NAME_IMPL(Skeleton, return Name);
    REFLECTABLE_FACTORY_IMPL(Skeleton);

    bool AnimationChannel::Retarget(const Skeleton* skel)
    {
        return Target.Resolve(skel);
    }

    void AnimationChannel::IDeserialize(istream& in, SETTINGS_T s)
    {
        ReadSerializable(in, Target, s);
        ReadArraySerializable<u16>(in, Frames, *this);
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
    REFLECTABLE_NAME_IMPL(AnimationChannel,
        return std::format("{}::{}", Target.Name, EChannelType.At(Type)->second)
    );
    REFLECTABLE_FACTORY_IMPL(AnimationChannel);

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

    void Animation::SetSkeleton(const Reference<gE::Skeleton>& skeleton)
    {
        if(!skeleton || _skeleton == skeleton) return;

        _skeleton = skeleton;
        for(u8 i = 0; i < Channels.Count(); i++)
            Channels[i].Retarget(_skeleton.GetPointer());
    }

    REFLECTABLE_ONGUI_IMPL(Animation,
        DrawField(Field{ "Name" }, Name, depth);
        DrawField(AssetDragDropField<Skeleton>{ "Skeleton" }, *this, depth, &Animation::GetSkeleton, &Animation::SetSkeleton);
        DrawField<const float>(ScalarField<float>{ "Length" }, Length, depth);
        DrawField(ArrayField<Field>{ "Channels" }, Channels, depth);
    );
    REFLECTABLE_NAME_IMPL(Animation, return Name);
    REFLECTABLE_FACTORY_IMPL(Animation);
}
