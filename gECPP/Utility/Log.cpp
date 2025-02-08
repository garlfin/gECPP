//
// Created by scion on 2/6/2025.
//

#include "Log.h"

#include <IMGUI/imgui.h>
#include <SDL3/SDL_messagebox.h>

#include "Macro.h"
#include "IMGUI/imgui_internal.h"

namespace gE
{
    void Log::Draw(bool* isOpen, bool first)
    {
        GE_ASSERTM(ImGui::GetCurrentContext(), "NO ACTIVE CONTEXT!");

        if(ImGui::Begin("Debug Menu", isOpen, ImGuiWindowFlags_MenuBar))
        {
            if(first)
                ImGui::SetWindowSize(ImVec2(700, 500));

            if(ImGui::Button("Clear"))
                Clear();

            ImGui::BeginMenuBar();
            ImGui::EndMenuBar();

            ImGui::BeginChild("debug.log", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
            {
                ImGui::TextUnformatted(_log.c_str());
                if(_scrollBottom)
                    ImGui::SetScrollHereY(1.f);
                _scrollBottom = false;
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    void Log::ShowError(const std::string_view& message)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", message.data(), nullptr);
    }
}
