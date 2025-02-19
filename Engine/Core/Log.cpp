//
// Created by scion on 2/6/2025.
//

#include "Log.h"

#include <IMGUI/imgui.h>
#include <SDL3/SDL_messagebox.h>

#include "Macro.h"

namespace gE
{
    void Log::Error(std::string_view message, bool fatal)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error!", message.data(), nullptr);
        if(fatal) std::terminate();
    }

    void Log::Warning(std::string_view message)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning!", message.data(), nullptr);
    }

    void Log::Info(std::string_view message)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Info", message.data(), nullptr);
    }
}
