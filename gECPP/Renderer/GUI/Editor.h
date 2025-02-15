//
// Created by scion on 2/11/2025.
//

#pragma once

#include <Prototype.h>

#include "Serializable/Serializable.h"
#include "Utility/Macro.h"

#define GE_EDITOR_HIERARCHY_FLAGS ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth
#define GE_EDITOR_TABLE_FLAGS ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoHostExtendX | ImGuiTableFlags_Resizable

namespace gE
{
#ifdef DEBUG
    template<class T, class OWNER_T>
    using GetterFunction = T (OWNER_T::*)() const;

    template<class T, class OWNER_T>
    using SetterFunction = void (OWNER_T::*)(T t);

    struct BaseFieldSettings
    {
    public:
        BaseFieldSettings() = default;
        BaseFieldSettings(const std::string_view& name) : Name(name) {};

        std::string_view Name = DEFAULT;
        std::string_view Tooltip = DEFAULT;
    };

    class Editor
    {
    public:
        explicit Editor(Window* window);

        void OnGUI();

        GET_SET_VALUE(bool, ConsoleOpen, _isConsoleOpen);
        GET_SET_VALUE(Entity*, SelectedEntity, _activeEntity);

        template<class T, class SETTINGS_T>
        static bool DrawField(const SETTINGS_T&, T&, u8 depth);

        template<class T, class SETTINGS_T, class OWNER_T>
        static bool DrawField(const SETTINGS_T&, OWNER_T&, u8 depth, GetterFunction<T, OWNER_T>, SetterFunction<T, OWNER_T>);

    private:
        void DrawLog();

        Window* _window = nullptr;
        Entity* _activeEntity = nullptr;
        bool _isConsoleOpen = false;
        size_t _oldLogSize = 0;
    };
#endif
}

#include "Editor.inl"
