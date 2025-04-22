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

            ImGui::TextUnformatted("Turn:");
            ShowRadio("Left", -3, *owner.GetCoasterType().SmallTurn, true, false);
            ShowRadio("Straight", 0, *owner.GetCoasterType().Straight, false);
            ShowRadio("Right", 3, *owner.GetCoasterType().SmallTurn, false);

            ImGui::TextUnformatted("Slope:");
            ShowRadio("Gentle Down", 5, *owner.GetCoasterType().GentleSlope, true, false);
            ShowRadio("Gentle Up", 6, *owner.GetCoasterType().GentleSlope, false);

            if(_preset)
                DrawField(gE::EnumField{ "Modifications", "", EETrackMod, (size_t) _preset->Modifications }, _modSelection, 0);

            if(ImGui::Button("Build") && _preset)
                new Track(&owner, owner.GetTrack().size() ? owner.GetTrack().back() : nullptr, _preset, _flipped);

        ImGui::End();
    }

    void CoasterEditor::ShowRadio(std::string_view label, int id, const TrackPreset& preset, bool flipped, bool sameLine)
    {
        if(sameLine)
            ImGui::SameLine();

        if(ImGui::RadioButton(label.data(), &turnSelection, id) || turnSelection == id)
        {
            _preset = &preset;
            _flipped = flipped;
        }
    }
}
