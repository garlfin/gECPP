//
// Created by scion on 3/3/2025.
//

#include "EntityInspector.h"

#include <Window.h>

namespace gE::Editor
{
    EntityInspector::EntityInspector(Editor* editor) : Window(editor, "Entity Inspector")
    {
        SetShortcut({ Key::LControl, Key::LShift, Key::E });
    }

    void EntityInspector::IOnEditorGUI()
    {
        if (_selected)
        {
            const std::string_view type = _selected->GetType() ? _selected->GetType()->Name : "gE::Entity (NO TYPE INFO)";

            ImGui::TextUnformatted(std::format("{} ({})", type, (void*) _selected).c_str());
            ImGui::Separator();
            _selected->OnEditorGUI(0);
        }
        else
            ImGui::TextUnformatted("No entity selected.");
    }

    EntityHierarchy::EntityHierarchy(Editor* editor, EntityInspector* inspector) : Window(editor, "Scene"),
        _inspector(inspector)
    {
        SetShortcut({ Key::LControl, Key::E });
    }

    void EntityHierarchy::IOnEditorGUI()
    {
        u8 reverseDepth = -1;
        for(const auto it : GetWindow().GetEntities().GetList())
        {
            Entity& entity = **it;

            const u8 curDepth = entity.GetTreeDepth();
            const u8 nextDepth = it->GetNext() ? (**it->GetNext())->GetTreeDepth() : -1;

            if(curDepth > reverseDepth && reverseDepth != -1)
                continue;

            if(curDepth <= reverseDepth) reverseDepth = -1;

            ImGuiTreeNodeFlags flag = GE_EDITOR_HIERARCHY_FLAGS | ImGuiTreeNodeFlags_Selected;
            if(curDepth >= nextDepth || !it->GetNext()) flag |= ImGuiTreeNodeFlags_Leaf;

            std::string name = std::format("{}##{}", entity.GetName(), (size_t) it);

            const bool open = ImGui::TreeNodeEx(name.c_str(), flag);
            if(ImGui::BeginPopupContextItem())
            {
                if(ImGui::Button("Delete"))
                {
                    entity.Destroy();
                    if(&entity == _inspector->GetSelected()) _inspector->SetSelected(nullptr);
                }
                ImGui::EndPopup();
            }
            if(!open)
            {
                reverseDepth = curDepth;
                continue;
            }

            if(ImGui::IsItemClicked()) _inspector->SetSelected(&entity);

            if(it->GetNext())
            {
                if (curDepth < nextDepth) continue;

                ImGui::TreePop(); // Pop this
                for (int i = 0; i < curDepth - nextDepth; i++)
                    ImGui::TreePop(); // How many steps back in the tree
            }
            else
                for (int i = 0; i <= curDepth; i++)
                    ImGui::TreePop();
        }
    }
};