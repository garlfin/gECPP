//
// Created by scion on 11/14/2024.
//

#pragma once

#include <filesystem>
#include <Prototype.h>
#include <Core/Array.h>
#include <Core/Mesh/Mesh.h>
#include <Core/Mesh/Skeleton.h>
#include <Core/Serializable/Asset.h>

namespace gE::Model
{
    struct GLTFResult
    {
        Array<Mesh> Meshes;
        Array<Skeleton> Skeletons;
        Array<Animation> Animations;
    };

    void ReadGLTF(Window*, const Path&, GLTFResult& result);
    void ReadGLTFAsFile(Window*, const std::filesystem::path&, Array<File>& files);
};
