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
        _subPreset(preset),
        _isFlipped(flip),
        _iterator(coaster->_track.end() - coaster->_track.begin())
    {
        if(_preset->TransitionPreset && previous && (previous->_preset != _preset || previous->_isFlipped != _isFlipped))
        {
            _subPreset = _preset->TransitionPreset.GetPointer();
        }

        if(previous && previous->_preset->TransitionPreset && (previous->_preset != _preset || previous->_isFlipped != _isFlipped))
        {
            _subPreset = previous->_preset->TransitionPreset.GetPointer();
            _isFlipped = previous->_isFlipped;
        }

        glm::vec3 offset = _subPreset->Offset;
        glm::vec3 exitPosition = _subPreset->ExitPosition;
        float exitRotation = _subPreset->ExitRotation;
        glm::mat4 transform = glm::mat4(1.f);

        if(_isFlipped)
        {
            offset = _subPreset->FlipTransform * glm::vec4(offset, 1.f);
            exitPosition = _subPreset->FlipTransform * glm::vec4(exitPosition, 1.f);
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
        if(_isFlipped)
            transform *= _subPreset->FlipTransform;

        coaster->_spline.AddSpline(_subPreset->Spline, transform);
        coaster->_track.push_back(this);
    }

    Track::~Track()
    {

    }

    Coaster::Coaster(gE::Window* window, const Reference<const CoasterType>& type) : Entity(window),
        _type(type),
        _editor(this),
        _splineRenderer(this, type->Track.Mesh, _spline)
    {
        _splineRenderer.SetMaterial(0, type->Track.Material);
        _splineRenderer.SetEnableDebugView(true);
    }

    Coaster::~Coaster()
    {
        for(Track* track : _track)
            delete track;
    }
}
