//
// Created by scion on 2/11/2025.
//

#include "Editor.h"

#include <windows.h>

#include "GUI.h"
#include "Window/Window.h"

namespace gE
{
#ifdef GE_ENABLE_IMGUI
    Editor::Editor(Window* window) : _window(window)
    {
    }

    void Editor::OnGUI()
    {
        if(_window->GetKeyboard().GetKey(Key::C) == KeyState::Pressed)
            _isConsoleOpen = !_isConsoleOpen;

        if(!_isConsoleOpen) return;

        DrawLog();

        if(ImGui::Begin("OBJECT INFO:", &_isConsoleOpen))
        {
            if (_activeObject && _activeObject->GetType())
                for (const TypeSystem::Field& field : _activeObject->GetType()->Fields)
                    DrawField(field);
            else
                ImGui::TextUnformatted("No object active.");

            ImGui::End();
        }
    }

    void Editor::DrawField(const TypeSystem::Field& field) const
    {
        const char* label = field.Name.data();

        switch(field.Type)
        {
        case FieldType::Bool:
            ImGui::Checkbox(label, (bool*) field.AtOffset(_activeObject));
            break;
        case FieldType::I32:
            ImGui::InputInt(label, (int*) field.AtOffset(_activeObject));
            break;
        case FieldType::U32:
            int result;
            ImGui::InputInt(label, &result);
            if(result < 0) result = 0;
            *(int*)field.AtOffset(_activeObject) = result;
            break;
        case FieldType::Float:
            ImGui::InputFloat(label, (float*) field.AtOffset(_activeObject));
            break;
        default:
            ImGui::TextUnformatted(std::format("Unknown field type:\nNAME:  {}\nTYPE: {}\nOFFSET: {}\n", field.Name, (u8) field.Type, field.Offset).c_str());
        }
    }

    void Editor::DrawLog()
    {
        GE_ASSERTM(ImGui::GetCurrentContext(), "NO ACTIVE CONTEXT!");

        if(!_isConsoleOpen) return;
        if(ImGui::Begin("Debug Menu", &_isConsoleOpen, ImGuiWindowFlags_MenuBar))
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
