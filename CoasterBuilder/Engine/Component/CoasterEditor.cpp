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
       // auto& tracks = GetOwner().GetTrack().GetList();
        //Track& lastTrack = ***tracks.GetLast();

        ImGui::Begin("Roller Coaster Construction", nullptr, ImGuiWindowFlags_MenuBar);
            ImGui::BeginMenuBar();
               // const size_t trackCount = tracks.Size();
               // ImGui::TextUnformatted(std::format("{}\tTrack Count: {}", GetOwner().GetName(), trackCount).c_str());
            ImGui::EndMenuBar();

        //DrawField(gE::EnumField{ "Type", "", ETrackType, (size_t) lastTrack.GetPiece().NextTypes }, _type, 0);

        const TrackPiece& piece = owner.GetCoasterType().GetPiece(_type);

        DrawField(gE::EnumField{ "Modifications", "", ETrackMod, (size_t) piece.Modifications }, _flag, 0);

       // if(ImGui::Button("Build"))
        //    new Track(&GetWindow(), GetOwner(), &piece, _flag, &lastTrack);

        ImGui::End();
    }
}
