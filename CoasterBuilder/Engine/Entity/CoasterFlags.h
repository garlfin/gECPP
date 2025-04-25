//
// Created by scion on 4/11/2025.
//

#pragma once

namespace Coaster
{
    CONSTEXPR_GLOBAL vec3 GridSize
    {
        2.f,
        2.f * std::sqrt(3.f) / 3.f,
        2.f,
    };

    CONSTEXPR_GLOBAL vec3 Up
    {
        0.f,
        1.f,
        0.f
    };

    inline quat TileRotationToQuat(float rotation)
    {
        return quat(radians(vec3(0.f, rotation, 0.f)));
    }

    CONSTEXPR_GLOBAL float GentleAngle = 30.f;
    CONSTEXPR_GLOBAL float SteepAngle = 60.f;

    enum class ETrackType : u16
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

    ENUM_OPERATOR(ETrackType, |);
    ENUM_OPERATOR(ETrackType, &);

    REFLECTABLE_ENUM(Normal, ETrackType, 7,
        ENUM_PAIR(ETrackType::Straight, "Straight"),
        ENUM_PAIR(ETrackType::Turn, "Small Turn"),
        ENUM_PAIR(ETrackType::BigTurn, "Big Turn"),
        ENUM_PAIR(ETrackType::HalfTurn, "Half Turn"),
        ENUM_PAIR(ETrackType::GentleSlope, "Gentle Slope"),
        ENUM_PAIR(ETrackType::SteepSlope, "Steep Slope"),
        ENUM_PAIR(ETrackType::Special, "Special"),
    );

    enum class ETrackMod : u8
    {
        None = 0,
        Station = 1 << 0,
        ChainLift = 1 << 1,
        DriveTire = 1 << 2,
        Brakes = 1 << 3,
        BlockBrakes = 1 << 4,
        All = UINT8_MAX
    };

    ENUM_OPERATOR(ETrackMod, |);
    ENUM_OPERATOR(ETrackMod, &);
    REFLECTABLE_ENUM(Normal, ETrackMod, 6,
        ENUM_PAIR(ETrackMod::None, "None"),
        ENUM_PAIR(ETrackMod::Station, "Station"),
        ENUM_PAIR(ETrackMod::ChainLift, "Chain Lift"),
        ENUM_PAIR(ETrackMod::DriveTire, "Drive Tires"),
        ENUM_PAIR(ETrackMod::Brakes, "Brakes"),
        ENUM_PAIR(ETrackMod::BlockBrakes, "Block Brakes"),
    );

    enum class Direction : u8
    {
        Left,
        Right
    };
}