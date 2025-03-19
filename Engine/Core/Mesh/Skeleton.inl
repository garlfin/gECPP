//
// Created by scion on 3/17/2025.
//

#pragma once

#include "Skeleton.h"

namespace gE
{
    inline Bone* BoneReference::Resolve(const Skeleton* skeleton)
    {
        if (Pointer || Name.empty() || !skeleton) return Pointer;

        Pointer = skeleton->FindBone(Name, Location);
        Location = Pointer - skeleton->Bones.begin();

        return Pointer;
    }

    inline void BoneReference::Set(const Skeleton* skel, Bone& bone)
    {
        Pointer = &bone;
        Name = bone.Name;
        Optimize(skel);
    }

    inline void BoneReference::Set(const Skeleton& skel, std::string_view name)
    {
        Pointer = skel.FindBone(name);
        Name = name;
        Optimize(&skel);
    }

    inline void BoneReference::Optimize(const Skeleton* skel)
    {
        if(!Pointer || !skel) return;

        Location = skel->GetIndex(*Pointer);
    }

    inline const Frame* AnimationChannel::GetFrame(float time) const
    {
        return std::lower_bound(Frames.begin(), Frames.end(), time, [](const Frame& frame, float time){ return frame.Time < time; });
    }
}
