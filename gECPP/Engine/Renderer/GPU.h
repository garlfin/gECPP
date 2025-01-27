//
// Created by scion on 1/27/2025.
//

#pragma once

#include <GLM/vec3.hpp>

#include <Engine/Math/Math.h>
#include <Engine/Window/WindowState.h>
#include <Graphics/Buffer/VAO.h>
#include <Graphics/Texture/Texture.h>

#define API_MAX_INSTANCE 64
#define API_MAX_LIGHT 4
#define API_MAX_CUBEMAP 4

namespace GPU
{
    struct Camera
    {
        glm::vec3 Position;
        u32 Frame;
        glm::vec2 Planes; // Near, Far
        glm::ivec2 Size; // Viewport Size
        glm::vec3 Parameters; // Up to implementation
        float FrameDelta;
        glm::vec4 PipelineParameters; // Up to implementation

        GPU_ALIGN handle ColorTexture;
        handle DepthTexture;

        glm::mat4 PreviousViewProjection;
        glm::mat4 Projection;
        glm::mat4 View[6];
    };

    enum class LightType : u32
    {
        Directional,
        Point,
        Spot,
        Area
    };

    struct Light
    {
        glm::mat4 ViewProjection;
        glm::vec3 Position;
        LightType Type;
        glm::vec3 Color;
        u32 PackedSettings;
        glm::vec2 Planes;
        handle Depth;
    };

    enum class CubemapType : u32
    {
        Infinite,
        AABB,
        Sphere,
        None = Infinite
    };

    struct Cubemap
    {
        glm::vec3 Position;
        float BlendRadius;
        glm::vec3 Scale;
        CubemapType Type;
        GPU_ALIGN handle Color;
    };

    struct ObjectInfo
    {
        glm::mat4 Model;
        glm::mat4 PreviousModel;
        glm::mat3x4 Normal;
    };

    struct Scene
    {
        gE::RenderFlags State;
        GPU_ALIGN u32 InstanceCount[API_MAX_MULTI_DRAW];
        GPU_ALIGN ObjectInfo Objects[API_MAX_INSTANCE];
    };

    struct Lighting
    {
        u32 LightCount = 1;
        u32 CubemapCount = 1;

        handle Skybox;
        ColorHarmonic SkyboxIrradiance;

        GPU_ALIGN Light Lights[API_MAX_LIGHT];
        GPU_ALIGN Cubemap Cubemaps[API_MAX_CUBEMAP];
    };
}