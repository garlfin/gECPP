//
// Created by scion on 3/17/2025.
//

#pragma once

#include "Skeleton.h"

namespace gE
{
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
}
