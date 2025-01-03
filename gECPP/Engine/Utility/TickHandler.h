//
// Created by scion on 10/31/2024.
//

#pragma once

#include <Engine/Math/Math.h>

struct TickHandler
{
public:
    TickHandler() = default;
    explicit TickHandler(double requiredDelta, bool strict = true) : RequiredDelta(requiredDelta), _strict(strict) {};
    explicit TickHandler(int hertz, bool strict = true) : RequiredDelta(1.0 / hertz), _strict(strict) {}

    double RequiredDelta = 0.0;

    GET_CONST(float, LerpFactor, _lerpFactor);
    GET_CONST(double, NextTickTime, _previousTime + GetTickDelta());
    GET_CONST(double, TickDelta, RequiredDelta);
    GET_CONST(int, TickRate, (int) (1.0 / RequiredDelta));
    GET_CONST(double, Delta, _delta);

    bool ShouldTick(double time);
    bool ShouldTickDelta(double delta);

private:
    float _lerpFactor = DEFAULT;
    double _previousTime = DEFAULT;
    double _delta = DEFAULT;
    bool _strict = true;
};

inline bool TickHandler::ShouldTickDelta(const double delta)
{
    _delta += delta;
    const double adjDelta = glm::min(_delta - RequiredDelta, RequiredDelta);

    if(!RequiredDelta)
    {
        _lerpFactor = 1.0;
        return true;
    }

    _lerpFactor = _delta / RequiredDelta;
    _lerpFactor = std::min(_lerpFactor, 1.f);
    if(_lerpFactor >= 1 - FLT_EPSILON) _lerpFactor = adjDelta / RequiredDelta;

    if(_delta < RequiredDelta) return false;

    _previousTime += RequiredDelta + adjDelta;
    _delta = adjDelta;

    return true;
}


inline bool TickHandler::ShouldTick(const double time)
{
    const double delta = time - _previousTime;
    const double adjDelta = glm::min(delta - RequiredDelta, RequiredDelta);

    if(!RequiredDelta)
    {
        _lerpFactor = 1.0;
        return true;
    }

    _lerpFactor = delta / RequiredDelta;
    _lerpFactor = std::min(_lerpFactor, 1.f);
    if(_lerpFactor >= 1 - FLT_EPSILON) _lerpFactor = adjDelta / RequiredDelta;

    if(delta < RequiredDelta) return false;

    _previousTime = time - adjDelta;
    _delta = delta;

    return true;
}
