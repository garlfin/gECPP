#define MAX_OBJECT 64
#define MAX_LIGHT 4
#define MAX_CUBEMAP 4

#define TEXTURE_HANDLE vec2

#define LIGHT_NONE 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_POINT 2
#define LIGHT_SPOT 3
#define LIGHT_AREA 4

struct Light
{
    mat4 ViewProjection;
    vec3 Color;
    uint Type;
    vec2 Settings;
    BINDLESS_TEXTURE(TEXTURE_HANDLE, Depth);
};

struct Cubemap
{
    vec3 Position;
    float BlendRadius;
    vec3 Scale;
    BINDLESS_TEXTURE(samplerCube, Color);
};

struct SceneData
{
    Light Lights[MAX_LIGHT];
    Cubemap Cubemaps[MAX_CUBEMAP];
    uint InstanceCount;
    mat4 Model[MAX_OBJECT];
    mat3 Normal[MAX_OBJECT];
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

#if defined(FRAGMENT) && !defined(GL_ARB_bindless_texture)
uniform sampler2D Lights[MAX_LIGHTS];
uniform samplerCube Cubemaps[MAX_CUBEMAPS];
#endif

uint ViewIndex = gl_InstanceID / Scene.InstanceCount;
uint ModelIndex = gl_InstanceID % Scene.InstanceCount;

