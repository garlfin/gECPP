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
struct _IPLContext_t;
struct _IPLHRTF_t;
struct _IPLBinauralEffect_t;

namespace gE
{
    class Sound
    {
    public:
        Sound() = default;
        explicit Sound(FMOD_STUDIO_EVENTINSTANCE* instance);

        OPERATOR_MOVE_NOSUPER(Sound, ~Sound,
            _instance = o._instance;
            o._instance = nullptr;
        );

        OPERATOR_COPY_PROTO(Sound);

        void Play() const;
        void Pause() const;
        void Stop() const;
        void SetTime(float) const;

        void SetUniform(std::string_view, float) const;
        void SetUniform(std::string_view, i32) const;
        void SetUniform(std::string_view, std::string_view) const;

        void SetPosition(const TransformData& transform);

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

    class Speaker : public Component
    {
    public:
        explicit Speaker(Entity* owner);

        void OnInit() override {};
        void OnUpdate(float delta) override;
        void SetSound(std::string_view name);

        const Sound* operator->() const { return &_sound; }

    private:
        Sound _sound = DEFAULT;
    };

    class SoundManager final : public ComponentManager<Speaker>
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
        _IPLContext_t* _steamAudioContext;
        _IPLHRTF_t* _steamAudioHRTF;

        std::vector<SoundBank> _banks;
    };
}
