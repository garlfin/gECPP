//
// Created by scion on 2/11/2025.
//

#pragma once

#include <Core/Macro.h>
#include <Entity/EditorCamera.h>

#include "AssetInspector.h"
#include "EntityInspector.h"

namespace gE::Editor
{
    class EditorLog final : public Window
    {
    public:
        explicit EditorLog(Editor*);

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
        bool OnRender();

        GET_SET(bool, IsOpen, _isOpen);
        GET_SET(bool, IsRunning, _isRunning);
        GET_CONST(gE::Window&, Window, *_window);
        GET_CONST(const Viewport&, Viewport, _viewport);

    private:
        gE::Window* _window = nullptr;
        Entity* _activeEntity = nullptr;
        API::Texture* _viewportTexture = nullptr;
        Viewport _viewport;

        EditorLog _log;
        AssetInspector _assetInspector;
        AssetManager _assetManager;
        EntityInspector _entityInspector;
        EntityHierarchy _entityHierarchy;

        EditorCamera _camera;

        bool _isOpen = false;
        bool _isRunning = true;
    };
#endif
}
