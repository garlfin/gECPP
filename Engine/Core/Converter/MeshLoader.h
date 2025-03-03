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
    void Read(Window*, const std::filesystem::path&, Array<Mesh>& meshesOut, Array<Skeleton>& skeletons);
    void ReadAsFile(Window*, const std::filesystem::path&, Array<File>& files);
};
