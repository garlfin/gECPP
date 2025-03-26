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
    struct GLTFImportSettings
    {
        float BoneScale = 1.f;
        bool FlipTangents = false;

    #ifdef GE_ENABLE_IMGUI
        void OnEditorGUI(u8 depth);
    #endif
    };

    struct GLTFResult
    {
        Array<Mesh> Meshes;
        Array<Skeleton> Skeletons;
        Array<Animation> Animations;
    };

    void ReadGLTF(Window*, const Path&, const GLTFImportSettings&, GLTFResult&);
    void ReadGLTFAsFile(Window*, const Path&, const GLTFImportSettings&, Array<File>& files);
};
