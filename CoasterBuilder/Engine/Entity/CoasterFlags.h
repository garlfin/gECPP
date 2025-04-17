//
// Created by scion on 4/11/2025.
//

#pragma once

namespace Coaster
{
    CONSTEXPR_GLOBAL glm::vec3 GridSize
    {
        2.f,
        2.f * std::sqrt(3.f) / 3.f,
        2.f,
    };

    CONSTEXPR_GLOBAL float GentleAngle = 30.f;
    CONSTEXPR_GLOBAL float SteepAngle = 60.f;

    enum class TrackType : u16
    {
        Straight = 1 << 0,
        Turn = 1 << 1,
        BigTurn = 1 << 2,
        HalfTurn = 1 << 3,
        GentleSlope = 1 << 4,
        SteepSlope = 1 << 5,
        Transition = 1 << 6,
        Special = 1 << 7,
        All = UINT16_MAX
    };

    ENUM_OPERATOR(TrackType, |);
    ENUM_OPERATOR(TrackType, &);
    REFLECTABLE_ENUM(Normal, TrackType, 7,
        ENUM_PAIR(TrackType::Straight, "Straight"),
        ENUM_PAIR(TrackType::Turn, "Small Turn"),
        ENUM_PAIR(TrackType::BigTurn, "Big Turn"),
        ENUM_PAIR(TrackType::HalfTurn, "Half Turn"),
        ENUM_PAIR(TrackType::GentleSlope, "Gentle Slope"),
        ENUM_PAIR(TrackType::SteepSlope, "Steep Slope"),
        ENUM_PAIR(TrackType::Special, "Special"),
    );

    enum class TrackMod : u8
    {
        None = 0,
        Station = 1 << 0,
        ChainLift = 1 << 1,
        DriveTire = 1 << 2,
        Brakes = 1 << 3,
        BlockBrakes = 1 << 4,
        All = UINT8_MAX
    };

    ENUM_OPERATOR(TrackMod, |);
    ENUM_OPERATOR(TrackMod, &);
    REFLECTABLE_ENUM(Normal, TrackMod, 6,
        ENUM_PAIR(TrackMod::None, "None"),
        ENUM_PAIR(TrackMod::Station, "Station"),
        ENUM_PAIR(TrackMod::ChainLift, "Chain Lift"),
        ENUM_PAIR(TrackMod::DriveTire, "Drive Tires"),
        ENUM_PAIR(TrackMod::Brakes, "Brakes"),
        ENUM_PAIR(TrackMod::BlockBrakes, "Block Brakes"),
    );

    enum class Direction : u8
    {
        Left,
        Right
    };
}