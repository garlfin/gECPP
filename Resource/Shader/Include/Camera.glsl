#include "Bindless.glsl"

struct CameraData
{
    vec3 Position;
    uint Frame;
    vec2 ClipPlanes;
    vec2 Parameters;

    BINDLESS_TEXTURE(sampler2D, Color);
    BINDLESS_TEXTURE(sampler2D, Depth);

    mat4 PreviousViewProjection;
    mat4 Projection;
    mat4 View[6];
};

#if defined(FRAGMENT_SHADER) && !defined(EXT_BINDLESS)
uniform sampler2D CameraDepth;
uniform sampler2D CameraColor;
#endif

#ifndef CAMERA_UNIFORM_LOCATION
#define CAMERA_UNIFORM_LOCATION 1
#endif

#ifndef CAMERA_UNIFORM_LAYOUT
#define CAMERA_UNIFORM_LAYOUT std140
#endif

layout(CAMERA_UNIFORM_LAYOUT, binding = CAMERA_UNIFORM_LOCATION) uniform CameraUniform
{
    CameraData Camera;
};

