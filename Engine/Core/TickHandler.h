//
// Created by scion on 10/31/2024.
//

#pragma once

#include <Core/Math/Math.h>

struct TickHandler
{
public:
    TickHandler() = default;
    explicit TickHandler(double requiredDelta) : RequiredDelta(requiredDelta) {};
    explicit TickHandler(int hertz) : RequiredDelta(1.0 / hertz) {}

    double RequiredDelta = 0.0;

    GET_CONST(float, LerpFactor, _lerpFactor);
    GET_CONST(double, NextTickTime, _previousTime + GetTickDelta());
    GET_CONST(double, TickDelta, RequiredDelta);
    GET_CONST(u32, TargetTickRate, (u32) (1.0 / RequiredDelta));
    GET_CONST(u32, TickRate, (u32) ceil(1.0 / _delta));
    GET_CONST(double, Delta, _delta);
    GET_CONST(u64, Tick, _tick);

    // Only valid after tick.
    GET_CONST(double, Time, _previousTime);

    bool ShouldTick(double time);
    bool ShouldTickDelta(double delta);

private:
    float _lerpFactor = DEFAULT;
    double _previousTime = DEFAULT;
    double _delta = DEFAULT;
    u64 _tick = 0;
};

inline bool TickHandler::ShouldTickDelta(const double delta)
{
    _delta += delta;
    const double adjDelta = min(_delta - RequiredDelta, RequiredDelta);

    if(!RequiredDelta)
    {
        _lerpFactor = 1.0;
        return true;
    }

    _lerpFactor = _delta / RequiredDelta;
    _lerpFactor = std::min(_lerpFactor, 1.f);

    if(adjDelta < 0.0) return false;

    _tick++;
    _lerpFactor = (float) (adjDelta / RequiredDelta);
    _previousTime += RequiredDelta + adjDelta;
    _delta = adjDelta;

    return true;
}


inline bool TickHandler::ShouldTick(const double time)
{
    const double delta = time - _previousTime;
    const double adjDelta = std::fmod(delta, RequiredDelta);//::min(delta - RequiredDelta, RequiredDelta);
    const double timePast = delta - RequiredDelta;

    if(RequiredDelta == 0.0)
    {
        _lerpFactor = 1.0;
        return true;
    }

    _lerpFactor = (float) std::min(adjDelta / RequiredDelta, 1.0);

    if(timePast < 0.0) return false;

    _tick++;
    _previousTime = time - adjDelta;
    _delta = delta;

    return true;
}
