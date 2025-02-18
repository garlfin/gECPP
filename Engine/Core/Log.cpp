//
// Created by scion on 2/6/2025.
//

#include "Log.h"

#include <IMGUI/imgui.h>
#include <SDL3/SDL_messagebox.h>

#include "Macro.h"

namespace gE
{
    void Log::FatalError(const std::string_view message)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Error!", message.data(), nullptr);
        std::terminate();
    }
}
