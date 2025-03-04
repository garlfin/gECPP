//
// Created by scion on 3/3/2025.
//

#pragma once

#include "Window.h"

#include <Core/Serializable/Asset.h>
#include <SDL3/SDL_dialog.h>

namespace gE::Editor
{
    constexpr inline std::array Filters
    {
        SDL_DialogFileFilter("Textures: .pvr", "pvr"),
        SDL_DialogFileFilter("3D Models: .dae, .fbx, .obj, .gltf, .glb", "dae;fbx;obj;gltf;glb")
    };

    enum class LoadAssetMode
    {
        ImportTexture = 0,
        ImportMesh = 1,
        LoadFile,
        None = -1
    };

    struct LoadingAsset
    {
        std::mutex Mutex = DEFAULT;
        Path Path = DEFAULT;
        LoadAssetMode Mode = LoadAssetMode::None;
    };

    class AssetInspector final : public Window
    {
    public:
        explicit AssetInspector(Editor*);

        GET_SET(const File*, Selected, _selected);

    protected:
        void IOnEditorGUI() override;

    private:
        const File* _selected = DEFAULT;
    };

    class AssetManager final : public Window
    {
    public:
        AssetManager(Editor*, AssetInspector*);

        GET(LoadingAsset&, Loading, _loading);

    protected:
        void IOnEditorGUI() override;

    private:
        static void LoadFileCallback(LoadingAsset*, const char* const* paths, int filter);
        static void ImportFileCallback(LoadingAsset*, const char* const* paths, int filter);

        AssetInspector* _inspector = DEFAULT;
        u16 _iconSize = 128;
        LoadingAsset _loading = DEFAULT;
    };
}
