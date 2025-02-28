//
// Created by scion on 2/6/2025.
//

#include "Log.h"

#include <IMGUI/imgui.h>
#include <SDL3/SDL_messagebox.h>

#include "Macro.h"

namespace gE
{
    void Log::FatalError(std::string_view message, std::string_view title)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.data(), message.data(), nullptr);
        std::terminate();
    }

    void Log::Error(std::string_view message, std::string_view title)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.data(), message.data(), nullptr);
    }

    void Log::Warning(std::string_view message, std::string_view title)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, title.data(), message.data(), nullptr);
    }

    void Log::Info(std::string_view message, std::string_view title)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.data(), message.data(), nullptr);
    }
}
