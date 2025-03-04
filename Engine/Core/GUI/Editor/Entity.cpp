//
// Created by scion on 3/3/2025.
//

#include "Entity.h"

namespace gE::Editor
{
    EntityInspector::EntityInspector(Editor*) {}
    void EntityInspector::IOnEditorGUI() {}
    Hierarchy::Hierarchy(Editor*, EntityInspector*) {}
    void Hierarchy::IOnEditorGUI() {}
};