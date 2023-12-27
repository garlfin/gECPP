#include "Bindless.glsl"

#define MAX_OBJECTS 64
#define MAX_LIGHTS 4
#define MAX_CUBEMAPS 4

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2
#define LIGHT_AREA 3

#define CUBEMAP_NONE 0
#define CUBEMAP_AABB 1
#define CUBEMAP_SPHERE 2

#define ENABLE_DEPTH (1 << 0)
#define ENABLE_COLOR (1 << 1)
#define ENABLE_VOXEL_WRITE (1 << 2)
#define ENABLE_JITTER (1 << 3)
#define ENABLE_SSEFFECTS (1 << 4)
#define ENABLE_SPECULAR (1 << 5)
#define ENABLE_FACE_CULL (1 << 6)

struct Light
{
    mat4 ViewProjection;
    vec3 Position;
    uint Type;
    vec3 Color;
    uint PackedSettings;
    vec2 Planes;
    BINDLESS_TEXTURE(TEXTURE_HANDLE, Depth);
};

struct Cubemap
{
    vec3 Position;
    float BlendRadius;
    vec3 Scale;
    uint Type;
    BINDLESS_TEXTURE(samplerCube, Color);
};

struct SceneData
{
    uint InstanceCount;
    uint State;
    mat4 Model[MAX_OBJECTS];
    mat4 PreviousModel[MAX_OBJECTS];
    mat3 Normal[MAX_OBJECTS];
};

struct LightingData
{
    uint LightCount;
    uint CubemapCount;

    BINDLESS_TEXTURE(samplerCube, Skybox);

    Light Lights[MAX_LIGHTS];
    Cubemap Cubemaps[MAX_CUBEMAPS];
};

#ifndef SCENE_UNIFORM_LOCATION
    #define SCENE_UNIFORM_LOCATION 0
#endif

#ifndef SCENE_UNIFORM_LAYOUT
    #define SCENE_UNIFORM_LAYOUT std140
#endif

layout(SCENE_UNIFORM_LAYOUT, binding = SCENE_UNIFORM_LOCATION) uniform SceneUniform
{
    SceneData Scene;
};

#ifndef LIGHT_UNIFORM_LOCATION
    #define LIGHT_UNIFORM_LOCATION 2
#endif

#ifndef LIGHT_UNIFORM_LAYOUT
    #define LIGHT_UNIFORM_LAYOUT std140
#endif

layout(LIGHT_UNIFORM_LAYOUT, binding = LIGHT_UNIFORM_LOCATION) uniform LightingUniform
{
    LightingData Lighting;
};

#if defined(FRAGMENT_SHADER) && !defined(GL_ARB_bindless_texture)
    uniform sampler2D Lights[MAX_LIGHTS];
    uniform samplerCube Cubemaps[MAX_CUBEMAPS];
#endif

#ifdef VERTEX_SHADER
    uint ViewIndex = gl_InstanceID / Scene.InstanceCount;
    uint ModelIndex = gl_InstanceID % Scene.InstanceCount;
#endif
