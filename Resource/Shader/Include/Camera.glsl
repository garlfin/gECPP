#include "Bindless.glsl"
#include "Math.glsl"

struct CameraData
{
    vec3 Position;
    uint Frame;
    vec2 Planes;
    vec2 Size;
    vec3 Parameters;
    float FrameDelta;

    BINDLESS_TEXTURE(sampler2D, Color);
    BINDLESS_TEXTURE(sampler2D, Depth);

    mat4 PreviousViewProjection;
    mat4 Projection;
    mat4 View[6];
};

#if defined(FRAGMENT_SHADER) && !defined(GL_ARB_bindless_texture)
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

vec3 PerspectiveToUV(vec4 proj)
{
    proj.xyz /= proj.w;
    proj.xy = proj.xy * 0.5 + 0.5;

    return vec3(proj.xy, LinearizeDepthNDC(proj.z, Camera.Planes));
}

