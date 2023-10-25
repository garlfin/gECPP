#include "Bindless.glsl"

struct CameraData
{
    vec3 Position;
    vec2 ClipPlanes;
    float FOV;
    uint Stage;

    BINDLESS_TEXTURE(sampler2D, Depth);
    BINDLESS_TEXTURE(sampler2D, Color);

    mat4 Projection;
    mat4 PreviousViewProjection;
    mat4 View[6];
};

#if defined(FRAGMENT) && !defined(GL_ARB_bindless_texture)
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

