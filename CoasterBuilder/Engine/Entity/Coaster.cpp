//
// Created by scion on 4/11/2025.
//

#include "Coaster.h"

namespace Coaster
{
    void TrackPreset::IDeserialize(istream& in, SETTINGS_T s) {}
    void TrackPreset::ISerialize(ostream& out) const {}

    REFLECTABLE_ONGUI_IMPL(TrackPreset, )
    REFLECTABLE_FACTORY_IMPL(TrackPreset);

    void CoasterType::IDeserialize(istream& in, SETTINGS_T s) {}
    void CoasterType::ISerialize(ostream& out) const {}

    const TrackPreset* CoasterType::GetPreset(ETrackType type) const
    {
        switch(type)
        {
        case ETrackType::Straight: return Straight.GetPointer();
        case ETrackType::Turn: return SmallTurn.GetPointer();
        case ETrackType::BigTurn: return BigTurn.GetPointer();
        case ETrackType::HalfTurn: return HalfTurn.GetPointer();
        default: return nullptr;
        }
    }

    const TrackAppearance* CoasterType::GetModAppearance(ETrackMod mod) const
    {
        return &Track;
    }

    REFLECTABLE_ONGUI_IMPL(CoasterType,)
    REFLECTABLE_FACTORY_IMPL(CoasterType);

    Track::Track(Coaster* coaster, const Track* previous, const TrackPreset* preset, bool flip) :
        _coaster(coaster),
        _preset(preset),
        _isFlipped(flip),
        _iterator(coaster->_track.end() - coaster->_track.begin())
    {
        glm::vec3 offset = preset->Offset;
        glm::vec3 exitPosition = preset->ExitPosition;
        float exitRotation = preset->ExitRotation;
        glm::mat4 transform = glm::mat4(1.f);

        if(flip)
        {
            offset = preset->FlipTransform * glm::vec4(offset, 1.f);
            exitPosition = preset->FlipTransform * glm::vec4(exitPosition, 1.f);
            exitRotation *= -1.f;
        }

        _exitPosition = exitPosition;
        _exitRotation = exitRotation;

        if(previous)
        {
            const glm::quat rot = TileRotationToQuat(previous->_exitRotation);
            offset = previous->_exitPosition + rot * offset;
            _exitPosition = previous->_exitPosition + rot * _exitPosition;
            _exitRotation += previous->_exitRotation;
        }

        transform = glm::translate(transform, offset);
        if(previous)
            transform *= glm::toMat4(TileRotationToQuat(previous->_exitRotation));
        if(flip)
            transform *= preset->FlipTransform;


        coaster->_spline.AddSpline(preset->Spline, transform);
        coaster->_track.push_back(this);
    }

    Track::~Track()
    {

    }

    Coaster::Coaster(gE::Window* window, const Reference<const CoasterType>& type) : Entity(window),
        _type(type),
        _editor(this),
        _splineRenderer(this, DEFAULT, _spline)
    {
        _splineRenderer.SetEnableDebugView(true);
    }

    Coaster::~Coaster()
    {
        for(Track* track : _track)
            delete track;
    }
}
