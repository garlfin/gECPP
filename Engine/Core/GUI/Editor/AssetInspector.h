//
// Created by scion on 3/3/2025.
//

#pragma once

#include <map>

#include "Window.h"
#include "Icon.h"

#include <Core/Serializable/Asset.h>
#include <SDL3/SDL_dialog.h>

#include "Core/Converter/MeshLoader.h"

namespace gE::Editor
{
    CONSTEXPR_GLOBAL std::array Filters
    {
        SDL_DialogFileFilter("Texture: .pvr", "pvr"),
        SDL_DialogFileFilter("Model: .gltf, .glb", "gltf;glb")
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
        GET_SET(Reference<SpriteSheet>, IconSheet, _iconSpriteSheet);

        void AddIcon(const Type<gE::Window*>& type, const Sprite& sprite);

    protected:
        void IOnEditorGUI() override;

    private:
        static void LoadFileCallback(LoadingAsset*, const char* const* paths, int filter);
        static void ImportFileCallback(LoadingAsset*, const char* const* paths, int filter);

        void LoadPendingFile();

        AssetInspector* _inspector = DEFAULT;
        u16 _iconSize = 128;
        LoadingAsset _loading = DEFAULT;

        Reference<SpriteSheet> _iconSpriteSheet;
        std::map<const Type<gE::Window*>*, Sprite> _icons;
        Sprite _defaultIcon;
        Model::GLTFImportSettings _gltfImportSettings = DEFAULT;
    };
}
