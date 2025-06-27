#extension GL_ARB_shader_viewport_layer_array : require

#include "Bindless.glsl"
#include "SphericalHarmonics.glsl"

#define DIV_CEIL(X, Y) (((X) + (Y) - 1) / (Y))

#extension GL_ARB_shader_draw_parameters : require
#extension GL_ARB_gpu_shader_int64 : enable

#define MAX_OBJECTS 64
#define MAX_LIGHTS 4
#define MAX_CUBEMAPS 4
#define MAX_MULTI_DRAW 8

#define LIGHT_NONE 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_POINT 2
#define LIGHT_SPOT 3
#define LIGHT_AREA 4

#define CUBEMAP_NONE 0
#define CUBEMAP_AABB 1
#define CUBEMAP_SPHERE 2
#define CUBEMAP_INFINITE 3

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

struct ObjectInfo
{
    mat4 Model;
    mat4 PreviousModel;
    mat3 Normal;
    uint Flags;
};

struct SceneData
{
    uint State;
    uvec4 InstanceCount[DIV_CEIL(MAX_MULTI_DRAW, 4)];
    ObjectInfo Objects[MAX_OBJECTS];
};

struct ObjectLighting
{
    uint LightCount;
    uint CubemapCount;

    Light Lights[MAX_LIGHTS];
    Cubemap Cubemaps[MAX_CUBEMAPS];
};

struct LightingData
{
    BINDLESS_TEXTURE(samplerCube, Skybox);
    ColorHarmonic SkyboxIrradiance;

    Light Sun;
    ObjectLighting Objects[MAX_OBJECTS];
};

#ifndef SCENE_UNIFORM_LOCATION
    #define SCENE_UNIFORM_LOCATION 0
#endif

layout(std140, binding = SCENE_UNIFORM_LOCATION) buffer SceneUniform
{
    SceneData Scene;
};

#ifndef LIGHT_UNIFORM_LOCATION
    #define LIGHT_UNIFORM_LOCATION 2
#endif

#ifndef LIGHT_UNIFORM_LAYOUT
    #define LIGHT_UNIFORM_LAYOUT std140
#endif

layout(LIGHT_UNIFORM_LAYOUT, binding = LIGHT_UNIFORM_LOCATION) buffer LightingUniform
{
    LightingData Lighting;
};

#ifdef VERTEX_SHADER
    uint InstanceCount = Scene.InstanceCount[gl_DrawID / 4][gl_DrawID % 4];
    uint ViewIndex = gl_InstanceID / InstanceCount;
    uint ObjectIndex = gl_BaseInstance + gl_InstanceID % InstanceCount;

    out flat uint ObjectIndexIn;
    out flat uint ViewIndexIn;
#endif

#ifdef FRAGMENT_SHADER
    in flat uint ObjectIndexIn;
    in flat uint ViewIndexIn;
    uint ViewIndex = ViewIndexIn;
#endif

#define RENDER_MODE_GEOMETRY 1
#define RENDER_MODE_FRAGMENT 2

#define WRITE_MODE_NONE 0
#define WRITE_MODE_DEPTH 1
#define WRITE_MODE_COLOR 2

#define DEPTH_MODE_NORMAL 0
#define DEPTH_MODE_RADIAL 1

#define VOXEL_MODE_READ 0
#define VOXEL_MODE_WRITE 1

#define RASTER_MODE_NORMAL 0
#define RASTER_MODE_CONSERVATIVE 1

const uint Scene_RenderMode = Scene.State & 2;
const uint Scene_WriteMode = Scene.State >> 2 & 2;
const uint Scene_DepthMode = Scene.State >> 4 & 1;
const uint Scene_VoxelWriteMode = Scene.State >> 5 & 1;
const uint Scene_RasterMode = Scene.State >> 6 & 1;
const uint Scene_InstanceMultiplier = Scene.State >> 8 & 7;
const bool Scene_UseLayer = bool(Scene.State >> 11 & 1);
const bool Scene_EnablePostProcess = bool(Scene.State >> 12 & 1);
const bool Scene_EnableFaceCull = bool(Scene.State >> 13 & 1);
const bool Scene_EnableDepthTest = bool(Scene.State >> 14 & 1);
const bool Scene_EnableSpecular = bool(Scene.State >> 15 & 1);

#ifdef VERTEX_SHADER
    void Scene_Setup()
    {
        if(Scene_UseLayer)
            gl_Layer = int(ViewIndex);
        else
            gl_ViewportIndex = int(ViewIndex);

        ObjectIndexIn = ObjectIndex;
        ViewIndexIn = ViewIndex;
    }
#endif

#define OBJECT_LIGHTING Lighting.Objects[ObjectIndexIn]
#define OBJECT_INFO Scene.Objects[ObjectIndexIn];