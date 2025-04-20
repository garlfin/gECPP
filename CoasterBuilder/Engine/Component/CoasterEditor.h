//
// Created by scion on 4/11/2025.
//

#pragma once

#include <Component/Behavior.h>
#include <CoasterBuilder/Engine/Entity/CoasterFlags.h>

namespace Coaster
{
    struct TrackPreset;
    class Coaster;

    class CoasterEditor : public gE::TypedBehavior<Coaster>
    {
    public:
        explicit CoasterEditor(Coaster* coaster);

        void OnInit() override {};

        void OnGUI(float delta) override;

    private:
        int turnSelection = DEFAULT;
        int slopeSelection = DEFAULT;
        ETrackMod _modSelection = DEFAULT;

        const TrackPreset* _preset = nullptr;
        bool _flipped = false;
    };
}
