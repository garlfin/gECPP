//
// Created by scion on 12/13/2024.
//

#pragma once

#include <Core/Macro.h>
#include <Core/Math/Math.h>

namespace gE
{
    struct TransformData
    {
        union
        {
            glm::vec3 Location = glm::vec3(0.f);
            glm::vec3 Position;
        };
        glm::vec3 Scale = glm::vec3(1.f);
        glm::quaternion Rotation = glm::identity<glm::quat>();

        NODISCARD ALWAYS_INLINE glm::vec3 Forward() const { return Rotation * glm::vec3(0, 0, -1); }
        NODISCARD ALWAYS_INLINE glm::vec3 Up() const { return Rotation * glm::vec3(0, 1, 0); }
        NODISCARD ALWAYS_INLINE glm::vec3 Right() const { return Rotation * glm::vec3(1, 0, 0); }
        GET_CONST(glm::mat3, RotationMatrix, glm::toMat3(Rotation));

        static TransformData mix(const TransformData& a, const TransformData& b, float factor);

        glm::mat4 ToMat4() const;
    };
}