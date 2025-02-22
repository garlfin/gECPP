//
// Created by scion on 2/20/2025.
//

#pragma once

#include <Component/Component.h>
#include <FMOD/fmod_studio.h>

#define GE_FMOD_MAX_CHANNELS 32

namespace gE
{
    class SoundComponent : public Component
    {
    public:
        explicit SoundComponent(Entity* owner);
    };

    class Speaker;
    class Listener;

    class SoundManager final : public ComponentManager<SoundComponent>
    {
    public:
        explicit SoundManager(Window* window);

        void OnUpdate(float delta) override;

        ~SoundManager() override;

    private:
        FMOD_STUDIO_SYSTEM* _system;
    };
}
