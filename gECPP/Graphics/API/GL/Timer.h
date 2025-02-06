//
// Created by scion on 12/22/2024.
//

#pragma once

#include <GLAD/glad.h>
#include <Graphics/API/GL/GL.h>
#include <Math/Math.h>

namespace GL
{
    class Timer : public GLObject
    {
    public:
        explicit Timer(gE::Window* window) : GLObject(window)
        {
            glCreateQueries(GL_TIME_ELAPSED, 1, &ID);
        }

        void Start() const
        {
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
            glBeginQuery(GL_TIME_ELAPSED, ID);
        }

        // Returns time in MS
        NODISCARD float End() const
        {
            u64 result;

            glMemoryBarrier(GL_ALL_BARRIER_BITS);
            glEndQuery(GL_TIME_ELAPSED);
            glGetQueryObjectui64v(ID, GL_QUERY_RESULT, &result);

            return (double) result * US_TO_MS;
        }

        void Bind() const override {};

        ~Timer() override
        {
            glDeleteQueries(1, &ID);
        }
    };
}
