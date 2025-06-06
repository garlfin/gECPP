//
// Created by scion on 4/11/2025.
//

#pragma once

#include <Window.h>
#include <CoasterBuilder/Engine/Component/CoasterEditor.h>
#include "CoasterFlags.h"
#include "Component/MeshRenderer/SplineRenderer.h"
#include "Core/Math/Spline.h"

namespace Coaster
{
    struct TrackPreset final : public Serializable<gE::Window*>
    {
        SERIALIZABLE_PROTO(TrackPreset, Serializable);

    public:
        Spline<Dimension::D3D> Spline;
        ETrackType NextTypes = ETrackType::All;
        ETrackMod Modifications = ETrackMod::None;
        vec3 Offset = DEFAULT;
        vec3 ExitPosition = DEFAULT;
        float ExitRotation = DEFAULT;
        mat4 FlipTransform = mat4(1.f);
        bool ReverseSplineOnFlip = false;

        Pointer<TrackPreset> TransitionPreset;
    };

    struct TrackAppearance
    {
        Reference<gE::Mesh> Mesh;
        Reference<gE::Material> Material;
    };

    struct CoasterType final : public Serializable<gE::Window*>
    {
        SERIALIZABLE_PROTO(CoasterType, Serializable);

    public:
        std::string Name;

        TrackAppearance Track;

        Reference<TrackPreset> Straight;

        Reference<TrackPreset> SmallTurn;
        Reference<TrackPreset> BigTurn;
        Reference<TrackPreset> HalfTurn;

        Reference<TrackPreset> GentleSlope;

        Array<Reference<TrackPreset>> SpecialPieces;

        const TrackPreset* GetPreset(ETrackType) const;
        const TrackAppearance* GetModAppearance(ETrackMod) const;
    };

    class Track
    {
    public:
        using Iterator = size_t;
        using SplineIterator = Spline<Dimension::D3D>::Iterator;
        using SplineRange = std::ranges::subrange<SplineIterator>;

        Track(Coaster* coaster, const Track* previous, const TrackPreset* preset, bool flip);

        GET_CONST(Iterator, Iterator, _iterator);
        GET_CONST(Coaster&, Coaster, *_coaster);
        GET_CONST(const TrackPreset&, Preset, *_preset);
        GET_CONST(const TrackPreset&, SubPreset, *_subPreset);
        GET_CONST(bool, IsFlipped, _isFlipped);

        ~Track();

    private:
        Coaster* _coaster = DEFAULT;
        const TrackPreset* _preset = DEFAULT;
        const TrackPreset* _subPreset = DEFAULT;
        bool _isFlipped = false;

        Iterator _iterator = DEFAULT;
        SplineRange _splineIterator = DEFAULT;

        vec3 _exitPosition = DEFAULT;
        float _exitRotation = 0.f;
    };

    class Coaster final : public gE::Entity
    {
    public:
        Coaster(gE::Window* window, const Reference<const CoasterType>& type);

        GET_CONST(const CoasterType&, CoasterType, *_type);

        GET_CONST(const Spline<Dimension::D3D>&, Spline, _spline);
        GET_CONST(const std::vector<Track*>&, Track, _track);

        ~Coaster() override;

        friend class Track;

    private:
        Reference<const CoasterType> _type;

        Spline<Dimension::D3D> _spline;
        std::vector<Track*> _track;

        CoasterEditor _editor;
        gE::SplineRenderer _splineRenderer;
    };
}
