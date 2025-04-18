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

    struct LoadingAssetData
    {
        std::mutex Mutex = DEFAULT;
        std::vector<Path> Paths = DEFAULT;
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

        GET(LoadingAssetData&, Loading, _loading);
        GET_SET(Reference<SpriteSheet>, IconSheet, _iconSpriteSheet);
        GET_SET(Path&, Path, _path);

        void AddIcon(const Type<gE::Window*>& type, const Sprite& sprite);

    protected:
        void IOnEditorGUI() override;

    private:
        static void LoadFileCallback(LoadingAssetData*, const char* const* paths, int filter);
        static void ImportFileCallback(LoadingAssetData*, const char* const* paths, int filter);

        void LoadPendingFile();

        AssetInspector* _inspector = DEFAULT;
        u16 _iconSize = 128;
        LoadingAssetData _loading = DEFAULT;

        Path _path = "/Resource/";

        Reference<SpriteSheet> _iconSpriteSheet;
        std::map<const Type<gE::Window*>*, Sprite> _icons;
        Sprite _defaultIcon;

        Model::GLTFImportSettings _gltfImportSettings = DEFAULT;
    };
}
