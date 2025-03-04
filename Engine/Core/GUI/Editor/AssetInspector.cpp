//
// Created by scion on 3/3/2025.
//

#include "AssetInspector.h"

#include <Core/Converter/MeshLoader.h>
#include <Core/Converter/PVR.h>
#include <Engine/Window.h>
#include <Vendor/IMGUI/imgui_internal.h>

namespace gE::Editor
{
    AssetInspector::AssetInspector(Editor* editor) : Window(editor, "Asset Inspector")
    {

    }

    void AssetInspector::IOnEditorGUI()
    {
        if(_selected)
        {
            const std::string_view type = _selected->GetFileType() ? _selected->GetFileType()->Name : "gE::Asset (NO TYPE INFO)";

            ImGui::TextUnformatted(std::format("{}\n\t{}", _selected->GetPath().string(), type).c_str());
            if(_selected->IsLoaded())
            {
                Asset& asset = _selected->Get();

                if(ImGui::Button("Reload File"))
                    _selected->ForceLoad();
                ImGui::SameLine();
                if(ImGui::Button("Save File"))
                {
                    if(asset.IsFree())
                        Log::Warning(std::format("Could not write to file because {} was freed.", _selected->GetPath().string()));
                    else
                    {
                        WriteSerializableToFile(_selected->GetPath(), asset);
                        Log::Info(std::format("Wrote file to {}", _selected->GetPath().string()));
                    }
                }
                if(!asset.IsFree())
                {
                    ImGui::SameLine();
                    if(ImGui::Button("Free CPU-Side Data"))
                        asset.Free();
                }

                ImGui::Separator();
                _selected->Get()->OnEditorGUI(0);
            }
            else
            {
                if(ImGui::Button("Load File"))
                    _selected->Load();
                ImGui::SameLine();
                ImGui::TextUnformatted("Asset is not loaded.");
                ImGui::Separator();
            }
        }
        else
            ImGui::TextUnformatted("No asset selected.");
    }

    AssetManager::AssetManager(Editor* editor, AssetInspector* inspector) : Window(editor, "Assets", ImGuiWindowFlags_MenuBar),
        _inspector(inspector)
    {
        API::Texture2D* sheetTex = (API::Texture2D*) PVR::Read(&GetWindow(), "Resource/Texture/editor_icons.pvr");
        _iconSpriteSheet = ref_create<SpriteSheet>(move(*sheetTex), glm::u16vec2(16));
        _defaultIcon = Sprite(_iconSpriteSheet, glm::u16vec2(0));
        SetShortcut({ Key::LControl, Key::A });

        AddIcon(GPU::ShaderSource::Type, Sprite(_iconSpriteSheet, glm::u16vec2(3, 0)));
        AddIcon(Material::Type, Sprite(_iconSpriteSheet, glm::u16vec2(1, 0)));

        delete sheetTex;
    }

    void AssetManager::LoadFileCallback(LoadingAsset* asset, const char* const* paths, int filter)
    {
        if(!*paths) return;

        asset->Mutex.lock();
        asset->Path = std::filesystem::relative(*paths);
        asset->Mode = LoadAssetMode::LoadFile;
        asset->Mutex.unlock();
    }

    void AssetManager::ImportFileCallback(LoadingAsset* asset, const char* const* paths, int filter)
    {
        if(!*paths) return;

        asset->Mutex.lock();
        asset->Path = std::filesystem::relative(*paths);
        asset->Mode = (LoadAssetMode) filter;
        asset->Mutex.unlock();
    }

    void AssetManager::LoadPendingFile()
    {
        gE::AssetManager& assetManager = GetWindow().GetAssets();

        _loading.Mutex.lock();
        if(_loading.Mode == LoadAssetMode::ImportTexture)
            assetManager.AddFile(PVR::ReadAsFile(&GetWindow(), _loading.Path));
        else if(_loading.Mode == LoadAssetMode::ImportMesh)
        {
            Array<File> files;
            Model::ReadAsFile(&GetWindow(), _loading.Path, files);

            for(File& file : files) assetManager.AddFile(std::move(file));
        }
        else if(_loading.Mode == LoadAssetMode::LoadFile)
        {
            const Path extension = _loading.Path.extension();
            if(extension == File::Extension)
                assetManager.LoadFile(_loading.Path, AssetLoadMode::PathsAndFiles);
            else
            {
                auto it = std::ranges::find_if(TypeSystem<gE::Window*>::GetTypes(), [&extension](const Type<gE::Window*>* type)
                {
                    return extension == type->Extension;
                });

                if(it != TypeSystem<gE::Window*>::GetTypes().end())
                    assetManager.AddFile(File(&GetWindow(), _loading.Path, *it))->Load();
            }
        }
        _loading.Mode = LoadAssetMode::None;
        _loading.Mutex.unlock();
    }

    void AssetManager::AddIcon(const Type<gE::Window*>& type, const Sprite& sprite)
    {
        _icons[&type] = sprite;
    }

    void AssetManager::IOnEditorGUI()
    {
        gE::AssetManager& assetManager = GetWindow().GetAssets();
        constexpr static u16 minScale = GE_EDITOR_ASSET_MIN_SCALE;
        constexpr static u16 maxScale = GE_EDITOR_ASSET_MAX_SCALE;

        LoadPendingFile();

        ImGui::BeginMenuBar();

            if(ImGui::Button("Load"))
                SDL_ShowOpenFileDialog((SDL_DialogFileCallback) LoadFileCallback, &_loading, nullptr, nullptr, 0, nullptr, false);
            if(ImGui::Button("Import"))
                SDL_ShowOpenFileDialog((SDL_DialogFileCallback) ImportFileCallback, &_loading, nullptr, Filters.data(), Filters.size(), nullptr, false);

            //if(ImGui::BeginPopupModal(55))

            ImGui::SetNextItemWidth(128.f);
            ImGui::SliderScalar("Icon Size", ImGuiDataType_U16, &_iconSize, &minScale, &maxScale, nullptr, ImGuiSliderFlags_AlwaysClamp);

        ImGui::EndMenuBar();

        const u32 windowSize = ImGui::GetContentRegionAvail().x - ImGui::GetCursorPos().x;
        const u16 width = _iconSize + GE_EDITOR_ASSET_PADDING * 2;
        const u8 columns = std::max<u8>(windowSize / width, 1);
        const gE::AssetManager::FILE_SET_T& assets = assetManager.GetFiles();

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + GE_EDITOR_ASSET_PADDING);
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(GE_EDITOR_ASSET_PADDING, GE_EDITOR_ASSET_PADDING));
        ImGui::BeginTable("##assets", columns, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoClip);
        for(size_t i = 0; auto& file : assets)
        {
            const u8 column = i % columns;
            if(!column) ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(column);

            const std::string fileName = file.GetPath().filename().replace_extension().string();
            const ImVec2 top = ImGui::GetCursorPos();
            const ImVec2 buttonSize = ImVec2(_iconSize + GE_EDITOR_ASSET_PADDING, _iconSize + ImGui::GetFont()->FontSize + GE_EDITOR_ASSET_PADDING / 2);

            ImGui::SetCursorPosX(top.x - GE_EDITOR_ASSET_PADDING / 2);
            std::string buttonLabel = std::format("##{}", fileName);

            ImGui::BeginGroup();
            if(ImGui::ButtonEx(buttonLabel.c_str(), buttonSize, ImGuiButtonFlags_PressedOnDoubleClick | ImGuiButtonFlags_AllowOverlap))
                _inspector->SetSelected(&file);

            ImGui::SetCursorPosX(top.x);
            ImGui::SetCursorPosY(top.y + GE_EDITOR_ASSET_PADDING / 2);

            if (!(file.IsLoaded() && file.Get()->OnEditorIcon(_iconSize)))
                if(auto it = _icons.find(file.GetFileType()->GetBaseType()); it != _icons.end())
                {
                    const Sprite& icon = it->second;
                    ImGui::Image((ImTextureID) (API::Texture*) &icon.SpriteSheet->Texture, ImVec2(_iconSize, _iconSize), icon.GetBottomUV(), icon.GetTopUV());
                }
                else
                    ImGui::Image((ImTextureID) (API::Texture*) &_defaultIcon.SpriteSheet->Texture, ImVec2(_iconSize, _iconSize), _defaultIcon.GetBottomUV(), _defaultIcon.GetTopUV());

            float textWidth = ImGui::CalcTextSize(fileName.c_str()).x;

            ImGui::SetCursorPosX(top.x + std::max((_iconSize - textWidth) / 2.f, 0.f));
            ImGui::TextUnformatted(fileName.c_str());
            ImGui::EndGroup();

            if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                const Reference<Asset>* ptr = &file.Get();
                ImGui::SetDragDropPayload(GE_EDITOR_ASSET_PAYLOAD, &ptr, sizeof(ptr));
                ImGui::TextUnformatted(fileName.c_str());
                ImGui::EndDragDropSource();
            }

            i++;
        }
        ImGui::EndTable();
        ImGui::PopStyleVar();
    }
}
