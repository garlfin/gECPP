//
// Created by scion on 2/20/2025.
//

#pragma once

#include <Component/Component.h>

#define GE_FMOD_MAX_CHANNELS 32

struct FMOD_STUDIO_BANK;
struct FMOD_STUDIO_SYSTEM;
struct FMOD_STUDIO_EVENTINSTANCE;
struct FMOD_STUDIO_EVENTDESCRIPTION;

namespace gE
{
    class SoundComponent : public Component
    {
    public:
        explicit SoundComponent(Entity* owner);
    };

    class Sound
    {
    public:
        Sound() = default;
        explicit Sound(FMOD_STUDIO_EVENTINSTANCE* instance);

        OPERATOR_MOVE_NOSUPER(Sound, ~Sound,
            _instance = o._instance;
            o._instance = nullptr;
        );

        void Play() const;

        ~Sound();

    private:
        FMOD_STUDIO_EVENTINSTANCE* _instance = nullptr;
    };

    struct SoundBank
    {
        FMOD_STUDIO_BANK* Bank;
        Path Path;

        bool operator==(const ::Path& path) const { return Path == path; }
    };

    class SoundManager final : public ComponentManager<SoundComponent>
    {
    public:
        explicit SoundManager(Window* window);

        void OnUpdate(float delta) override;

        void LoadBank(const Path& path);
        void UnloadBank(const Path& path);

        Sound GetSound(std::string_view name);

        ~SoundManager() override;

    private:
        void UnloadBank(const SoundBank& bank);

        FMOD_STUDIO_SYSTEM* _system;
        std::vector<SoundBank> _banks;
    };
}
