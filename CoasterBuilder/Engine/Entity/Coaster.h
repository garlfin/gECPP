//
// Created by scion on 4/11/2025.
//

#pragma once

#include <Window.h>
#include <CoasterBuilder/Engine/Component/CoasterEditor.h>
#include "CoasterFlags.h"
#include "Core/Math/Spline.h"

namespace Coaster
{
    struct TrackPiece final : public Serializable<gE::Window*>
    {
        SERIALIZABLE_PROTO("TRCK", 0, TrackPiece, Serializable);

    public:
        Array<std::pair<TrackMod, Reference<gE::Mesh>>> Meshes = DEFAULT;
        Reference<gE::Material> Material = DEFAULT;
        TrackType NextTypes = TrackType::All;
        TrackMod Modifications = TrackMod::None;
        i8 ExitRotation = 0;
        glm::vec3 ExitPosition = DEFAULT;

        Reference<gE::Mesh> GetMesh(TrackMod) const;
    };

    struct CoasterType final : public Serializable<gE::Window*>
    {
        SERIALIZABLE_PROTO("TRCK", 0, CoasterType, Serializable);

    public:
        std::string Name;

        TrackPiece Pieces[6];

        constexpr TrackPiece& GetPiece(TrackType type) { return Pieces[std::countr_zero((std::underlying_type_t<TrackType>) type)]; }
        constexpr const TrackPiece& GetPiece(TrackType type) const { return Pieces[std::countr_zero((std::underlying_type_t<TrackType>) type)]; }

        constexpr TrackPiece& Straight() { return GetPiece(TrackType::Straight); }
        constexpr TrackPiece& SmallTurn() { return GetPiece(TrackType::Turn); }
        constexpr TrackPiece& BigTurn() { return GetPiece(TrackType::BigTurn); }

        Array<TrackPiece> SpecialPieces;
    };

    class Coaster final : public gE::Entity
    {
    public:
        Coaster(gE::Window* window, const Reference<const CoasterType>& type);

        GET_CONST(const CoasterType&, CoasterType, *_type);

    private:
        Reference<const CoasterType> _type;
        Spline<Dimension::D3D> _spline;
        CoasterEditor _editor;
    };
}
