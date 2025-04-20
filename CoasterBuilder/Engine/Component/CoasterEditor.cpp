//
// Created by scion on 4/11/2025.
//

#include "CoasterEditor.h"
#include <CoasterBuilder/Engine/Entity/Coaster.h>

namespace Coaster
{
    CoasterEditor::CoasterEditor(Coaster* coaster) : TypedBehavior(coaster)
    {

    }

    void CoasterEditor::OnGUI(float delta)
    {
        Coaster& owner = GetOwner();

        ImGui::Begin("Roller Coaster Construction", nullptr, ImGuiWindowFlags_MenuBar);
            ImGui::BeginMenuBar();
               const size_t trackCount = owner.GetTrack().size();
               ImGui::TextUnformatted(std::format("{}\tTrack Count: {}", GetOwner().GetName(), trackCount).c_str());
            ImGui::EndMenuBar();

            if(ImGui::RadioButton("LEFT", &turnSelection, -3))
            {
                _preset = &*owner.GetCoasterType().SmallTurn;
                _flipped = true;
            }

            ImGui::SameLine();
            if(ImGui::RadioButton("STRAIGHT", &turnSelection, 0) || !turnSelection)
            {
                _preset = &*owner.GetCoasterType().Straight;
                _flipped = false;
            }

            ImGui::SameLine();
            if(ImGui::RadioButton("RIGHT", &turnSelection, 3))
            {
                _preset = &*owner.GetCoasterType().SmallTurn;
                _flipped = false;
            }

            if(_preset)
                DrawField(gE::EnumField{ "Modifications", "", EETrackMod, (size_t) _preset->Modifications }, _modSelection, 0);

            if(ImGui::Button("Build") && _preset)
                new Track(&owner, owner.GetTrack().size() ? owner.GetTrack().back() : nullptr, _preset, _flipped);

        ImGui::End();
    }
}
