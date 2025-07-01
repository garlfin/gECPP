//
// Created by scion on 3/3/2025.
//

#pragma once

#include <Prototype.h>
#include <Core/Input.h>
#include <Core/Macro.h>

#include "Settings.h"

namespace gE::Editor
{
    class Editor;

    class Window
    {
        DELETE_OPERATOR_CM(Window);
    public:
        explicit Window(Editor* editor, const std::string& name, ImGuiWindowFlags flags = ImGuiWindowFlags_None);

        GET_SET(bool, IsOpen, _isOpen);
        GET_CONST(Editor&, Editor, *_editor);
        GET_CONST(const Shortcut&, Shortcut, _shortcut);
        SET_XVAL(Shortcut, Shortcut, _shortcut);
        NODISCARD gE::Window& GetWindow() const;

        void OnEditorGUI();

        virtual ~Window() = default;

    protected:
        virtual void IOnEditorGUI() = 0;

    private:
        Editor* _editor;
        bool _isOpen = true;
        ImGuiWindowFlags _flags;
        std::string _name;
        Shortcut _shortcut = DEFAULT;
    };
}
