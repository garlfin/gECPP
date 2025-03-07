//
// Created by scion on 2/11/2025.
//

#pragma once

#include <Prototype.h>
#include <Core/Macro.h>
#include <Core/Serializable/Serializable.h>

#include "AssetInspector.h"
#include "EntityInspector.h"

namespace gE::Editor
{
    class EditorLog final : public Window
    {
    public:
         EditorLog(Editor*);

    protected:
         void IOnEditorGUI() override;

    private:
        size_t _oldLogSize = 0;
    };

#ifdef GE_ENABLE_EDITOR
    class Editor
    {
    public:
        explicit Editor(gE::Window* window);

        void OnGUI();

        GET_SET(bool, IsOpen, _isOpen);
        GET_SET(bool, IsRunning, _isRunning);
        GET_CONST(gE::Window&, Window, *_window);

    private:
        gE::Window* _window = nullptr;
        Entity* _activeEntity = nullptr;

        EditorLog _log;
        AssetInspector _assetInspector;
        AssetManager _assetManager;
        EntityInspector _entityInspector;
        EntityHierarchy _entityHierarchy;

        bool _isOpen = false;
        bool _isRunning = true;
    };
#endif
}
