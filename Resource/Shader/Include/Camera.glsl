#include "Bindless.glsl"
#include "Math.glsl"

#define MAX_VIEW 6

struct CameraData
{
    vec2 Planes;
    ivec2 Size;
    vec2 Parameters;
    float FrameDelta;
    uint Frame;
    vec4 PipelineParameters;

    BINDLESS_TEXTURE(sampler2D, Color);
    BINDLESS_TEXTURE(sampler2D, Depth);

    mat4 Projection;
    mat4 View[MAX_VIEW];
    mat4 PreviousView[MAX_VIEW];
    vec3 Position[MAX_VIEW];
};

#if defined(FRAGMENT_SHADER) && !defined(GL_ARB_bindless_texture)
    uniform sampler2D CameraDepth;
    uniform sampler2D CameraColor;
#endif

#ifndef CAMERA_UNIFORM_LOCATION
    #define CAMERA_UNIFORM_LOCATION 1
#endif

layout(std140, binding = CAMERA_UNIFORM_LOCATION) uniform CameraUniform
{
    CameraData Camera;
};

vec3 PerspectiveToUV(vec4 proj)
{
    proj.xyz /= proj.w;
    proj.xy = proj.xy * 0.5 + 0.5;

    return vec3(proj.xy, LinearizeDepthNDC(proj.z, Camera.Planes));
}

