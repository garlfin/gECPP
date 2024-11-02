//
// Created by scion on 10/31/2024.
//

#pragma once

#include <Engine/Math/Math.h>

struct TickHandler
{
public:
    TickHandler() = default;
    explicit TickHandler(u32 tickRate) : TickRate(tickRate) {};

    u32 TickRate = 0;

    GET_CONST(float, LerpFactor, _lerpFactor);
    GET_CONST(double, NextTickTime, _previousTime + GetTickDelta());
    GET_CONST(double, TickDelta, 1.0 / (double) TickRate);
    GET_CONST(double, Delta, _delta);

    bool ShouldTick(double time);

private:
    float _lerpFactor = DEFAULT;
    double _previousTime = DEFAULT;
    double _delta = DEFAULT;
};

inline bool TickHandler::ShouldTick(const double time)
{
    const double requiredDelta = GetTickDelta();
    const double delta = time - _previousTime;
    const double adjDelta = glm::min(delta - requiredDelta, requiredDelta);

    if(!TickRate)
    {
        _lerpFactor = 1.0;
        return true;
    }

    _lerpFactor = delta / requiredDelta;
    _lerpFactor = std::min(_lerpFactor, 1.f);
    if(_lerpFactor >= 1 - FLT_EPSILON) _lerpFactor = adjDelta / requiredDelta;

    if(delta < requiredDelta) return false;

    _previousTime = time - adjDelta;
    _delta = delta;

    return true;
}
