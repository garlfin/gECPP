//
// Created by scion on 2/11/2025.
//

#pragma once

#include <Prototype.h>

#include "Serializable/Serializable.h"
#include "Utility/Macro.h"

namespace gE
{
#ifdef DEBUG
    class Editor
    {
    public:
        explicit Editor(Window* window);

        void OnGUI();

        GET_SET_VALUE(bool, ConsoleOpen, _isConsoleOpen);
        GET_SET_VALUE(Entity*, SelectedEntity, _activeEntity);

    private:
        static void DrawField(Reflectable<Window*>&, const TypeSystem::Field&);
        void DrawLog();

        Window* _window = nullptr;
        Entity* _activeEntity = nullptr;
        bool _isConsoleOpen = false;
        size_t _oldLogSize = 0;
    };
#endif
}
