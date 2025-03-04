//
// Created by scion on 2/11/2025.
//

#include "Editor.h"

#include <Window.h>
#include "Window.h"
#include <Core/Converter/MeshLoader.h>
#include <Core/Converter/PVR.h>
#include <Vendor/IMGUI/imgui_internal.h>
#include <Vendor/SDL3/SDL_dialog.h>

#if defined(GE_ENABLE_EDITOR) && !defined(GE_ENABLE_IMGUI)
    #error IMGUI not enabled.
#endif

#ifdef GE_ENABLE_EDITOR
namespace gE::Editor
{
    Window::Window(Editor* editor, const std::string& name, ImGuiWindowFlags flags) :
        _editor(editor),
        _flags(flags),
        _name(name)
    {

    }

    gE::Window& Window::GetWindow() const
    {
        return _editor->GetWindow();
    }

    void Window::OnEditorGUI()
    {
        if(CheckShortcut()) _isOpen = !_isOpen;

        if(!_isOpen) return;

        if(ImGui::Begin(_name.c_str(), &_isOpen, _flags))
            IOnEditorGUI();
        ImGui::End();
    }

    bool Window::CheckShortcut() const
    {
        const KeyboardState& keyboard = GetWindow().GetKeyboard();

        u8 nullCount = 0;
        for(Key key : _shortcut)
            if((bool) key)
            {
                if(keyboard.GetKey(key) != KeyState::Pressed)
                    return false;
            }
            else nullCount++;

        return nullCount != _shortcut.size();
    }

    Editor::Editor(gE::Window* window) :
        _window(window),
        _assetInspector(this),
        _assetManager(this, &_assetInspector)
    {
    }

    void Editor::OnGUI()
    {
        KeyboardState& keyboard = _window->GetKeyboard();
        if(_window->GetMouse().GetIsEnabled() && !keyboard.GetIsFocused() && keyboard.GetKey(Key::C) == KeyState::Pressed)
            _isOpen = !_isOpen;

        _window->SetViewport(Viewport(_window->GetSize(), DEFAULT));

        if(!_isOpen) return;

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

        _assetInspector.OnEditorGUI();
        _assetManager.OnEditorGUI();
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
