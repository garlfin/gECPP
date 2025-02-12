//
// Created by scion on 2/11/2025.
//

#pragma once

#include <Prototype.h>

#include "Serializable/Serializable.h"
#include "Utility/Macro.h"

namespace gE
{
#ifdef GE_ENABLE_IMGUI
    class Editor
    {
    public:
        explicit Editor(Window* window);

        void OnGUI();

        GET_SET_VALUE(bool, ConsoleOpen, _isConsoleOpen);
        GET_SET_VALUE(Reflectable<Window*>*, ActiveObject, _activeObject);

    private:
        void DrawField(const TypeSystem::Field& field) const;
        void DrawLog();

        Window* _window = nullptr;
        Reflectable<Window*>* _activeObject = nullptr;
        bool _isConsoleOpen = false;
        size_t _oldLogSize = 0;
    };
#endif
}
