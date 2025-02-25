//
// Created by scion on 2/11/2025.
//

#include "Editor.h"

#include <Window.h>
#include <windows.h>
#include <IMGUI/imgui_internal.h>

#include "GUI.h"

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
        constexpr static u16 minScale = 16;
        constexpr static u16 maxScale = 512;

        if(ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_MenuBar))
        {
            ImGui::BeginMenuBar();
            if(ImGui::BeginMenu("View"))
            {
                ImGui::SetNextItemWidth(128.f);
                ImGui::SliderScalar("Icon Size", ImGuiDataType_U16, &_iconSize, &minScale, &maxScale, nullptr, ImGuiSliderFlags_AlwaysClamp);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();

            const u32 windowSize = ImGui::GetContentRegionAvail().x - ImGui::GetCursorPos().x;
            const u16 width = _iconSize + GE_EDITOR_ICON_PADDING * 2;
            const u8 columns = std::max<u8>(windowSize / width, 1);
            const AssetManager::FILE_SET_T& assets = _window->GetAssets().GetFiles();

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(GE_EDITOR_ICON_PADDING, GE_EDITOR_ICON_PADDING));
            ImGui::BeginTable("##assets", columns, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersV);
            for(size_t i = 0; auto& file : assets)
            {
                const u8 column = i % columns;
                if(!column) ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(column);

                float top = ImGui::GetCursorPosY();

                const std::string fileName = file.GetPath().filename().replace_extension().string();

                if(file.IsLoaded())
                    file.Get()->OnEditorIcon(_iconSize);
                ImGui::TextWrapped(fileName.c_str());

                float bottom = ImGui::GetCursorPosY();

                ImGui::SetCursorPosY(top);
                ImGui::Dummy(ImVec2(_iconSize, bottom - top));

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
