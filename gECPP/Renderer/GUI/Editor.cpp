//
// Created by scion on 2/11/2025.
//

#include "Editor.h"

#include <windows.h>

#include "GUI.h"
#include "IMGUI/imgui_internal.h"
#include "Window/Window.h"

namespace gE
{
#ifdef DEBUG
    Editor::Editor(Window* window) : _window(window)
    {
    }

    void Editor::OnGUI()
    {
        KeyboardState& keyboard = _window->GetKeyboard();
        if(_window->GetMouse().GetIsEnabled() && !keyboard.GetIsFocused() && keyboard.GetKey(Key::C) == KeyState::Pressed)
            _isConsoleOpen = !_isConsoleOpen;

        if(!_isConsoleOpen) return;

        keyboard.SetIsFocused(ImGui::GetIO().WantCaptureKeyboard);

        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

        DrawLog();

        if(ImGui::Begin("Entity Info", &_isConsoleOpen, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (_activeEntity)
            {
                GE_ASSERT(_activeEntity->GetType());
                ImGui::TextUnformatted(std::format("{} ({})", _activeEntity->GetType()->Name, (void*) _activeEntity).c_str());
                ImGui::NewLine();
                _activeEntity->OnEditorGUI(0);
            }
            else
                ImGui::TextUnformatted("No entity selected.");
            ImGui::End();
        }

        if(ImGui::Begin("Entity Browser", &_isConsoleOpen))
        {
            if(ImGui::TreeNodeEx("Scene", GE_EDITOR_HIERARCHY_FLAGS | ImGuiTreeNodeFlags_Leaf))
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
                    if(!ImGui::TreeNodeEx(name.c_str(), flag))
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
                ImGui::TreePop();
            }
            ImGui::End();
        }
    }

    void Editor::DrawLog()
    {
        GE_ASSERTM(ImGui::GetCurrentContext(), "NO ACTIVE CONTEXT!");

        if(!_isConsoleOpen) return;
        if(ImGui::Begin("Console:", &_isConsoleOpen, ImGuiWindowFlags_MenuBar))
        {
            if(ImGui::Button("Clear"))
                Log::Clear();

            ImGui::BeginMenuBar();
            ImGui::EndMenuBar();

            ImGui::BeginChild("debug.log", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
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
#endif
}
