//
// Created by scion on 3/3/2025.
//

#pragma once

#include "Window.h"

namespace gE::Editor
{
    class EntityInspector : public Window
    {
    public:
        explicit EntityInspector(Editor*);

    protected:
        void IOnEditorGUI() override;

    private:
        Entity* _selected = DEFAULT;
    };

    class Hierarchy : public Window
    {
    public:
        Hierarchy(Editor*, EntityInspector*);

    protected:
        void IOnEditorGUI() override;

    private:
        EntityInspector* _inspector;
    };
}
