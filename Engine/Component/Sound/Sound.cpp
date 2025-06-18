//
// Created by scion on 2/20/2025.
//

#include "Sound.h"

#include <FMOD/fmod_studio.h>

#include <Entity/Entity.h>
#include <Window.h>

namespace gE
{
    FMOD_VECTOR ToFMOD(const vec3& vec) { return FMOD_VECTOR(vec.x, vec.y, vec.z); }
    FMOD_3D_ATTRIBUTES ToFMOD(const TransformData& transform)
    {
        return FMOD_3D_ATTRIBUTES(
            ToFMOD(transform.Position),
            ToFMOD(vec3(0.f)),
            ToFMOD(transform.Forward()),
            ToFMOD(transform.Up())
        );
    }

    Speaker::Speaker(Entity* owner) : Component(owner, &owner->GetWindow().GetSounds())
    {

    }

    void Speaker::OnUpdate(float delta)
    {
        _sound.SetPosition(GetOwner().GetTransform().GetGlobalTransform());
    }

    void Speaker::SetSound(std::string_view name)
    {
        _sound = GetWindow().GetSounds().GetSound(name);
    }

    Sound::Sound(FMOD_STUDIO_EVENTINSTANCE* instance) : _instance(instance)
    {
    }

    OPERATOR_COPY_IMPL(Sound::, Sound, this->~Sound(),
        FMOD_STUDIO_EVENTDESCRIPTION* description;
        FMOD_Studio_EventInstance_GetDescription(_instance, &description);
        FMOD_Studio_EventDescription_CreateInstance(description, &_instance);
    );

    void Sound::Play() const
    {
        FMOD_Studio_EventInstance_Start(_instance);
    }

    void Sound::Pause() const
    {
        FMOD_Studio_EventInstance_SetPaused(_instance, true);
    }

    void Sound::Stop() const
    {
        FMOD_Studio_EventInstance_Stop(_instance, FMOD_STUDIO_STOP_ALLOWFADEOUT);
    }

    void Sound::SetTime(float time) const
    {
        FMOD_Studio_EventInstance_SetTimelinePosition(_instance, time);
    }

    void Sound::SetUniform(std::string_view name, float value) const
    {
        FMOD_Studio_EventInstance_SetParameterByName(_instance, name.cbegin(), value, false);
    }

    void Sound::SetUniform(std::string_view name, i32 value) const
    {
        FMOD_Studio_EventInstance_SetParameterByName(_instance, name.cbegin(), value, false);
    }

    void Sound::SetUniform(std::string_view name, std::string_view value) const
    {
        FMOD_Studio_EventInstance_SetParameterByNameWithLabel(_instance, name.cbegin(), value.cbegin(), false);
    }

    void Sound::SetPosition(const TransformData& transform)
    {
        FMOD_3D_ATTRIBUTES attributes = ToFMOD(transform);
        FMOD_Studio_EventInstance_Set3DAttributes(_instance, &attributes);
    }

    Sound::~Sound()
    {
        FMOD_Studio_EventInstance_Release(_instance);
    }

    SoundManager::SoundManager(Window* window) : ComponentManager(window)
    {
        FMOD_RESULT result = FMOD_Studio_System_Create(&_system, FMOD_VERSION);
        if(result != FMOD_OK)
            Log::FatalError("Failed to create FMOD Studio System.");

        result = FMOD_Studio_System_Initialize(
            _system,
            GE_FMOD_MAX_CHANNELS,
            FMOD_STUDIO_INIT_NORMAL,
            FMOD_INIT_3D_RIGHTHANDED,
            nullptr
        );
        if(result != FMOD_OK)
            Log::FatalError("Failed to initialize FMOD Studio System.");
    }

    void SoundManager::OnUpdate(float delta)
    {
        ComponentManager::OnUpdate(delta);

        const Transform& cameraTransform = GetWindow()->GetCameras().GetCurrentCamera()->GetCamera().GetOwner().GetTransform();
        const FMOD_3D_ATTRIBUTES listenerAttributes = ToFMOD(cameraTransform.GetGlobalTransform());

        FMOD_Studio_System_SetListenerAttributes(_system, 0, &listenerAttributes, nullptr);
        if(const FMOD_RESULT err = FMOD_Studio_System_Update(_system); err != FMOD_OK)
            Log::WriteLine("FMOD Update Error: {}",  (u32) err);
    }

    void SoundManager::LoadBank(const Path& path)
    {
        FMOD_STUDIO_BANK* _loadedBank;

        Path strBankPath = path;
        strBankPath.replace_extension(".strings.bank");
        if(FMOD_Studio_System_LoadBankFile(_system, strBankPath.string().c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &_loadedBank) != FMOD_OK)
            Log::WriteLine("Failed to strings bank \"{}\"", strBankPath.string());

        if(FMOD_Studio_System_LoadBankFile(_system, path.string().c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &_loadedBank) != FMOD_OK)
            Log::WriteLine("Failed to load bank \"{}\"", path.string());

#ifdef DEBUG
        int eventCount;
        FMOD_Studio_Bank_GetEventCount(_loadedBank, &eventCount);
        Array<FMOD_STUDIO_EVENTDESCRIPTION*> events(eventCount);

        FMOD_Studio_Bank_GetEventList(_loadedBank, events.Data(), events.Size(), nullptr);

        for(FMOD_STUDIO_EVENTDESCRIPTION* event : events)
        {
            char eventName[1024];
            FMOD_Studio_EventDescription_GetPath(event, eventName, 1024, nullptr);
            Log::WriteLine("Event \"{}\" found in \"{}\"", eventName, path.string());
        }
#endif

        _banks.emplace_back(_loadedBank, path);
    }

    void SoundManager::UnloadBank(const Path& path)
    {
        auto it = std::find(_banks.begin(), _banks.end(), path);
        if(it == _banks.end())
            return;

        UnloadBank(*it);
        _banks.erase(it);
    }

    Sound SoundManager::GetSound(std::string_view name)
    {
        FMOD_STUDIO_EVENTDESCRIPTION* eventDescription;
        if(FMOD_Studio_System_GetEvent(_system, name.cbegin(), &eventDescription) != FMOD_OK)
            Log::WriteLine("Could not load sound \"{}\"", name);

        FMOD_STUDIO_EVENTINSTANCE* instance;
        FMOD_Studio_EventDescription_CreateInstance(eventDescription, &instance);
        return Sound(instance);
    }

    void SoundManager::UnloadBank(const SoundBank& bank)
    {
        FMOD_Studio_Bank_Unload(bank.Bank);
    }

    SoundManager::~SoundManager()
    {
        for(SoundBank& bank : _banks)
            UnloadBank(bank);

        FMOD_Studio_System_Release(_system);
    }
}
