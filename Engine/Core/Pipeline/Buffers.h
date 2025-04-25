//
// Created by scion on 1/27/2025.
//

#pragma once

#include <GLM/vec3.hpp>

#include <Core/WindowState.h>
#include <Core/Math/Math.h>
#include <Graphics/Texture/Texture.h>

#define API_MAX_INSTANCE 64
#define API_MAX_LIGHT 4
#define API_MAX_CUBEMAP 4
#define API_MAX_MULTI_DRAW 8
#define API_MAX_VIEW 6

namespace GPU
{
    struct Camera
    {
        vec2 Planes; // Near, Far
        ivec2 Size; // Viewport Size
        vec2 Parameters; // Up to implementation
        float FrameDelta;
        u32 Frame;
        vec4 PipelineParameters; // Up to implementation

        GPU_ALIGN handle ColorTexture;
        handle DepthTexture;

        mat4 Projection;
        mat4 View[API_MAX_VIEW];
        mat4 PreviousView[API_MAX_VIEW];
        vec4 Position[API_MAX_VIEW];
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
        mat4 ViewProjection;
        vec3 Position;
        LightType Type;
        vec3 Color;
        u32 PackedSettings;
        vec2 Planes;
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
        vec3 Position;
        float BlendRadius;
        vec3 Scale;
        CubemapType Type;
        GPU_ALIGN handle Color;
    };

    struct ObjectFlags
    {
        bool IsDynamic : 1 = 0;
    };

    struct ObjectInfo
    {
        mat4 Model;
        mat4 PreviousModel;
        mat3x4 Normal;
        GPU_ALIGN ObjectFlags Flags = DEFAULT;
    };

    struct Scene
    {
        gE::RenderFlags State;
        GPU_ALIGN u32 InstanceCount[API_MAX_MULTI_DRAW];
        ObjectInfo Objects[API_MAX_INSTANCE];
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