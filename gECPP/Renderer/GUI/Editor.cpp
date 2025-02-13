//
// Created by scion on 2/11/2025.
//

#include "Editor.h"

#include <windows.h>

#include "GUI.h"
#include "IMGUI/imgui_internal.h"
#include "Window/Window.h"
#include "IMGUI/imgui_stdlib.h"

#define GE_EDITOR_HIERARCHY_FLAGS ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
#define GE_EDITOR_TABLE_FLAGS ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable

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

        if(ImGui::Begin("Entity Info", &_isConsoleOpen))
        {
            if (_activeEntity && _activeEntity->GetType())
                DrawReflectable(*_activeEntity, 0);
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

    void Editor::DrawField(Reflectable& object, const Field& field, u8 depth)
    {
        void* target = field.AtOffset(&object);
        const FieldType rawType = field.Type & ~FieldType::Mods;

        if((bool)(field.Type & FieldType::Pointer))
        {
            target = *(void**) target;
            ImGui::TextUnformatted(std::format("Pointer: {}", target).c_str());
            if(!target) return;
        }

        if(rawType == FieldType::Bool)
            ImGui::Checkbox("##Enabled", (bool*) target);
        else if(rawType == FieldType::I32)
            ImGui::InputInt("##Value", (int*) target);
        else if(rawType == FieldType::U32)
        {
            int result;
            ImGui::InputInt("##Value", &result);
            if(result < 0) result = 0;
            *(int*)field.AtOffset(&object) = result;
        }
        else if(rawType == FieldType::Float)
            ImGui::InputFloat("##Value", (float*) target);
        else if(rawType == FieldType::String)
            ImGui::InputText("##Value", (std::string*) target);
        else if(rawType == FieldType::Reflectable)
        {
            Reflectable& reflected = *(Reflectable*) target;
            if(reflected.GetType())
                DrawReflectable(reflected, depth + 1);
        }
        else
            ImGui::TextUnformatted(std::format("Unknown field type:\nNAME:  {}\nTYPE: {}\nOFFSET: {}\n", field.Name, (u8) field.Type, field.Offset).c_str());
    }

    void Editor::DrawReflectable(Reflectable& reflected, u8 depth)
    {
        const Type& type = *reflected.GetType();

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;
        if(depth <= 1) flags |= ImGuiTreeNodeFlags_DefaultOpen;

        if(!ImGui::TreeNodeEx(std::format("{}##{}", reflected.GetType()->Name, (void*) &reflected).c_str(), flags))
            return;

        reflected.OnEditorGUI();

        ImGui::BeginTable("entity_fields", 2, GE_EDITOR_TABLE_FLAGS);
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Value");

        for(const Field& field : type.Fields)
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(field.Name.data());
            ImGui::TableSetColumnIndex(1);
            DrawField(reflected, field, depth);
        }

        ImGui::EndTable();
        ImGui::TreePop();
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
