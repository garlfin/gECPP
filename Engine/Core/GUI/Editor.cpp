//
// Created by scion on 2/11/2025.
//

#include "Editor.h"

#include <Window.h>
#include <windows.h>
#include <Core/Converter/MeshLoader.h>
#include <Core/Converter/PVR.h>
#include <IMGUI/imgui_internal.h>
#include <SDL3/SDL_dialog.h>

#ifdef GE_ENABLE_EDITOR

#ifndef GE_ENABLE_IMGUI
    #error IMGUI not enabled.
#endif

namespace gE
{
    Editor::Editor(Window* window) : _window(window)
    {
    }

    void Editor::OnGUI()
    {
        KeyboardState& keyboard = _window->GetKeyboard();
        if(_window->GetMouse().GetIsEnabled() && !keyboard.GetIsFocused() && keyboard.GetKey(Key::C) == KeyState::Pressed)
            _isEditorOpen = !_isEditorOpen;

        _window->SetViewport(Viewport(_window->GetSize(), DEFAULT));

        if(!_isEditorOpen) return;

        keyboard.SetIsFocused(ImGui::GetIO().WantCaptureKeyboard);

        ImGuiID centralNodeID = ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode);
        ImGuiDockNode* centralNode = ImGui::DockBuilderGetCentralNode(centralNodeID);

        const Size2D offset = (Size2D) centralNode->Pos;
        const Size2D size = (Size2D) centralNode->Size;
        _window->SetViewport(Viewport(size, offset));

        DrawLog();
        DrawInspector();
        DrawHierarchy();
        DrawEntityDrawer();
        DrawAssetManager();
    }

    void Editor::DrawInspector()
    {
        if(ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (_activeEntity)
            {
                const std::string_view type = _activeEntity->GetType() ? _activeEntity->GetType()->Name : "gE::Entity (NO TYPE INFO)";

                ImGui::TextUnformatted(std::format("{} ({})", type, (void*) _activeEntity).c_str());
                ImGui::Separator();
                _activeEntity->OnEditorGUI(0);
            }
            else
                ImGui::TextUnformatted("No entity selected.");
        }
        ImGui::End();
    }

    void Editor::DrawHierarchy()
    {
        if(ImGui::Begin("Scene"))
        {
            u8 reverseDepth = -1;
            for(const auto it : _window->GetEntities().GetList())
            {
                Entity& entity = **it;

                const u8 curDepth = entity.GetTreeDepth();
                const u8 nextDepth = it->GetNext() ? (**it->GetNext())->GetTreeDepth() : -1;

                if(curDepth > reverseDepth && reverseDepth != -1)
                    continue;

                if(curDepth <= reverseDepth) reverseDepth = -1;

                ImGuiTreeNodeFlags flag = GE_EDITOR_HIERARCHY_FLAGS | ImGuiTreeNodeFlags_Selected;
                if(curDepth >= nextDepth || !it->GetNext()) flag |= ImGuiTreeNodeFlags_Leaf;

                std::string name = std::format("{}##{}", entity.GetName(), (size_t) it);

                const bool open = ImGui::TreeNodeEx(name.c_str(), flag);
                if(ImGui::BeginPopupContextItem())
                {
                    if(ImGui::Button("Delete"))
                    {
                        entity.Destroy();
                        if(&entity == _activeEntity) _activeEntity = nullptr;
                    }
                    ImGui::EndPopup();
                }
                if(!open)
                {
                    reverseDepth = curDepth;
                    continue;
                }

                if(ImGui::IsItemClicked()) _activeEntity = &entity;

                if(it->GetNext())
                {
                    if (curDepth < nextDepth) continue;

                    ImGui::TreePop(); // Pop this
                    for (int i = 0; i < curDepth - nextDepth; i++)
                        ImGui::TreePop(); // How many steps back in the tree
                }
                else
                    for (int i = 0; i <= curDepth; i++)
                        ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    void Editor::DrawAssetManager()
    {
        AssetManager& assetManager = _window->GetAssets();

        _loadingMutex.lock();
        if(_loadingFilter == GE_EDITOR_FILTER_TEXTURE)
            assetManager.AddFile(PVR::ReadAsFile(_window, _loadingPath));
        else if(_loadingFilter == GE_EDITOR_FILTER_MODEL)
        {
            Array<File> files;
            Model::ReadAsFile(_window, _loadingPath, files);

            for(File& file : files) assetManager.AddFile(std::move(file));
        }
        else if(_loadingFilter == GE_EDITOR_FILTER_FILE)
        {
            const Path extension = _loadingPath.extension();
            if(extension == File::Extension)
                assetManager.LoadFile(_loadingPath, AssetLoadMode::PathsAndFiles);
            else
            {
                auto it = std::ranges::find_if(TypeSystem<Window*>::GetTypes(), [&extension](const Type<Window*>* type)
                {
                    return extension == type->Extension;
                });

                if(it != TypeSystem<Window*>::GetTypes().end())
                    assetManager.AddFile(File(_window, _loadingPath, *it))->Load();
            }
        }
        _loadingFilter = -1;
        _loadingMutex.unlock();

        constexpr static u16 minScale = 64;
        constexpr static u16 maxScale = 512;

        if(ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_MenuBar))
        {
            ImGui::BeginMenuBar();

            if(ImGui::Button("Load"))
                SDL_ShowOpenFileDialog((SDL_DialogFileCallback) LoadFileCallback, this, nullptr, nullptr, 0, nullptr, false);
            if(ImGui::Button("Import"))
                SDL_ShowOpenFileDialog((SDL_DialogFileCallback) ImportFileCallback, this, nullptr, Filters.data(), Filters.size(), nullptr, false);

            //if(ImGui::BeginPopupModal(55))

            ImGui::SetNextItemWidth(128.f);
            ImGui::SliderScalar("Icon Size", ImGuiDataType_U16, &_iconSize, &minScale, &maxScale, nullptr, ImGuiSliderFlags_AlwaysClamp);

            ImGui::EndMenuBar();

            const u32 windowSize = ImGui::GetContentRegionAvail().x - ImGui::GetCursorPos().x;
            const u16 width = _iconSize + GE_EDITOR_ICON_PADDING * 2;
            const u8 columns = std::max<u8>(windowSize / width, 1);
            const AssetManager::FILE_SET_T& assets = assetManager.GetFiles();

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + GE_EDITOR_ICON_PADDING);
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(GE_EDITOR_ICON_PADDING, GE_EDITOR_ICON_PADDING));
            ImGui::BeginTable("##assets", columns, ImGuiTableFlags_SizingStretchSame);
            for(size_t i = 0; auto& file : assets)
            {
                const u8 column = i % columns;
                if(!column) ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(column);

                const std::string fileName = file.GetPath().filename().replace_extension().string();
                const ImVec2 top = ImGui::GetCursorPos();
                const ImVec2 buttonSize = ImVec2(_iconSize + GE_EDITOR_ICON_PADDING, _iconSize + ImGui::GetFont()->FontSize + GE_EDITOR_ICON_PADDING / 2);

                ImGui::SetCursorPosX(top.x - GE_EDITOR_ICON_PADDING / 2);
                std::string buttonLabel = std::format("##{}", fileName);

                ImGui::BeginGroup();
                if(ImGui::ButtonEx(buttonLabel.c_str(), buttonSize, ImGuiButtonFlags_PressedOnDoubleClick | ImGuiButtonFlags_AllowOverlap))
                    _activeAsset = &file;

                ImGui::SetCursorPosX(top.x);
                ImGui::SetCursorPosY(top.y + GE_EDITOR_ICON_PADDING / 2);

                if(file.IsLoaded())
                    file.Get()->OnEditorIcon(_iconSize);

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
        ImGui::End();

        if(ImGui::Begin("Asset Inspector"))
        {
            if(_activeAsset)
            {
                const std::string_view type = _activeAsset->GetFileType() ? _activeAsset->GetFileType()->Name : "gE::Asset (NO TYPE INFO)";

                ImGui::TextUnformatted(std::format("{}\n\t{}", _activeAsset->GetPath().string(), type).c_str());
                if(_activeAsset->IsLoaded())
                {
                    Asset& asset = _activeAsset->Get();

                    if(ImGui::Button("Reload File"))
                        _activeAsset->ForceLoad();
                    ImGui::SameLine();
                    if(ImGui::Button("Save File"))
                    {
                        if(asset.IsFree())
                            Log::Warning(std::format("Could not write to file because {} was freed.", _activeAsset->GetPath().string()));
                        else
                        {
                            WriteSerializableToFile(_activeAsset->GetPath(), asset);
                            Log::Info(std::format("Wrote file to {}", _activeAsset->GetPath().string()));
                        }
                    }
                    if(!asset.IsFree())
                    {
                        ImGui::SameLine();
                        if(ImGui::Button("Free CPU-Side Data"))
                            asset.Free();
                    }

                    ImGui::Separator();
                    _activeAsset->Get()->OnEditorGUI(0);
                }
                else
                {
                    if(ImGui::Button("Load File"))
                        _activeAsset->Load();
                    ImGui::SameLine();
                    ImGui::TextUnformatted("Asset is not loaded.");
                    ImGui::Separator();
                }
            }
            else
                ImGui::TextUnformatted("No asset selected.");
        }
        ImGui::End();
    }

    void Editor::LoadFileCallback(Editor* editor, const char* const* paths, int filter)
    {
        if(!paths) return;

        editor->_loadingMutex.lock();
        editor->_loadingPath = std::filesystem::relative(*paths);
        editor->_loadingFilter = GE_EDITOR_FILTER_FILE;
        editor->_loadingMutex.unlock();
    }

    void Editor::ImportFileCallback(Editor* editor, const char* const* paths, int filter)
    {
        if(!paths) return;

        editor->_loadingMutex.lock();
        editor->_loadingPath = std::filesystem::relative(*paths);
        editor->_loadingFilter = filter;
        editor->_loadingMutex.unlock();
    }

    void Editor::DrawLog()
    {
        GE_ASSERTM(ImGui::GetCurrentContext(), "NO ACTIVE CONTEXT!");

        if(ImGui::Begin("Console", nullptr, ImGuiWindowFlags_MenuBar))
        {
            if(ImGui::Button("Clear"))
                Log::Clear();

            ImGui::BeginMenuBar();
            ImGui::EndMenuBar();

            ImGui::BeginChild("log", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            {
                ImGui::TextUnformatted(Log::Get().data());
                if(Log::Get().size() != _oldLogSize)
                    ImGui::SetScrollHereY(1.f);
                _oldLogSize = Log::Get().size();
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void Editor::DrawEntityDrawer()
    {
        if(ImGui::Begin("Entity Drawer", nullptr, ImGuiWindowFlags_AlwaysVerticalScrollbar))
        {
            for(auto& type : TypeSystem<const EntityCreationSettings&>::GetTypes())
            {
                ImGui::TextUnformatted(type->Name.data());
            }
        }
        ImGui::End();
    }
}
#endif
