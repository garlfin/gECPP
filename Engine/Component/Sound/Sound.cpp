//
// Created by scion on 2/20/2025.
//

#include "Sound.h"

#include <Entity/Entity.h>
#include <Window.h>

namespace gE
{
    SoundComponent::SoundComponent(Entity* owner) : Component(owner, &owner->GetWindow().GetSounds()) {}

    SoundManager::SoundManager(Window* window) : ComponentManager(window)
    {
        FMOD_RESULT result = FMOD_Studio_System_Create(&_system, FMOD_VERSION);
        if(result != FMOD_OK)
            Log::Error("Failed to create FMOD Studio System.", true);

        result = FMOD_Studio_System_Initialize(
            _system,
            GE_FMOD_MAX_CHANNELS,
            FMOD_STUDIO_INIT_NORMAL,
            FMOD_INIT_NORMAL,
            nullptr
        );
        if(result != FMOD_OK)
            Log::Error("Failed to initialize FMOD Studio System.", true);
    }

    void SoundManager::OnUpdate(float delta)
    {
        ComponentManager::OnUpdate(delta);
        FMOD_Studio_System_Release(_system);
    }

    SoundManager::~SoundManager()
    {
        FMOD_Studio_System_Release(_system);
    }
}
