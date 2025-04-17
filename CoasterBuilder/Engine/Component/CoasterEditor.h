//
// Created by scion on 4/11/2025.
//

#pragma once

#include <Component/Behavior.h>
#include <CoasterBuilder/Engine/Entity/CoasterFlags.h>

namespace Coaster
{
    class Coaster;

    class CoasterEditor : public gE::TypedBehavior<Coaster>
    {
    public:
        explicit CoasterEditor(Coaster* coaster);

        void OnInit() override {};

        void OnGUI(float delta) override;

    private:
        TrackMod _flag = TrackMod::None;
        TrackType _type = TrackType::Straight;
    };
}
