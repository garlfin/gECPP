//
// Created by scion on 10/31/2024.
//

#pragma once

#include <Engine/Math/Math.h>
#include <GLFW/glfw3.h>

struct TickResult
{
    double Delta;
    double CurrentTime;
    bool ShouldTick;
};

template<bool STRICT = false>
struct TickHandler
{
public:
    TickHandler() = default;
    explicit TickHandler(u8 tickRate) : TickRate(tickRate) {};

    u8 TickRate = 0;

    TickResult ShouldTick()
    {
        const double requiredTime = 1.0 / TickRate;

        const double time = glfwGetTime();
        const double delta = time - _previousTime;

        if(delta < requiredTime) return { delta, time, false };

        if constexpr(STRICT)
            _previousTime = time - delta + requiredTime;
        else
            _previousTime = time;

        return { delta, time, true };
    }

private:
    double _previousTime = DEFAULT;
};
