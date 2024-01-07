#include "Bindless.glsl"

#ifndef VOXEL_TEXTURE_LOCATION
    #define VOXEL_TEXTURE_LOCATION 0
#endif

#ifndef VOXEL_DATA_LOCATION
    #define VOXEL_DATA_LOCATION 1
#endif

#ifndef VOXEL_TRACE_MAX_ITERATIONS
    #define VOXEL_TRACE_MAX_ITERATIONS 64
#endif

#ifndef EPSILON
    #define EPSILON 0.0001
#endif

#ifndef PIPELINE_COLOR_FORMAT
    #define PIPELINE_COLOR_FORMAT rgb10_a2
#endif

#ifndef PIPELINE_DATA_FORMAT
    #define PIPELINE_DATA_FORMAT r8ui
#endif

#ifndef VOXEL_COLOR_RANGE
    #define VOXEL_COLOR_RANGE 2.f
#endif

uniform layout(binding = VOXEL_TEXTURE_LOCATION, PIPELINE_COLOR_FORMAT) restrict image3D VoxelColorOut;

struct VoxelGridData
{
    vec3 Position;
    float Scale;
    BINDLESS_TEXTURE(sampler3D, Color);
};

#if defined(FRAGMENT_SHADER) && !defined(GL_ARB_bindless_texture)
uniform sampler3D VoxelColor;
uniform sampler3D VoxelData;
#endif

#ifndef VOXEL_UNIFORM_LOCATION
    #define VOXEL_UNIFORM_LOCATION 4
#endif

#ifndef VOXEL_UNIFORM_LAYOUT
    #define VOXEL_UNIFORM_LAYOUT std140
#endif

layout(VOXEL_UNIFORM_LAYOUT, binding = VOXEL_UNIFORM_LOCATION) uniform VoxelGridUniform
{
    VoxelGridData VoxelGrid;
};

struct Ray
{
    vec3 Position;
    float MaximumDistance;
    vec3 Direction;
};

struct RayResult
{
    vec3 Position;
    float Distance;
    vec3 Normal;
    bool Hit;
};

// Functions

vec4 PackColor(vec4);
vec4 UnpackColor(vec4);

RayResult Trace(Ray);
RayResult TraceOffset(Ray, vec3);

// Helper Functions
vec3 WorldToUV(vec3);
vec3 TexelToUV(vec3, uint); // CellCount
ivec3 UVToTexel(vec3, uint); // CellCount
ivec3 WorldToTexel(vec3, uint); // CellCount
vec3 WorldToAlignedUV(vec3, uint); // CellCount

vec3 AlignWorldToCell(vec3, uint); // CellCount
float CrossCell(inout vec3, vec3, uint); // CellCount

// Implementation
vec3 WorldToUV(vec3 pos)
{
    vec3 uv = (pos - VoxelGrid.Position) / VoxelGrid.Scale;
    return uv * 0.5 + 0.5;
}

vec3 TexelToUV(ivec3 texel, uint cellCount) { return (vec3(texel) + 0.5) * ((VoxelGrid.Scale * 2.0) / cellCount); }
ivec3 UVToTexel(vec3 uv, uint cellCount) { return ivec3(uv * cellCount); }
ivec3 WorldToTexel(vec3 pos, uint cellCount) { return UVToTexel(WorldToUV(pos), cellCount); }

vec4 PackColor(vec4 color)
{
    color.rgb = pow(color.rgb / VOXEL_COLOR_RANGE, vec3(1.0 / 2.2));
    return color;
}

vec4 UnpackColor(vec4 color)
{
    color.rgb = pow(color.rgb, vec3(2.2)) * VOXEL_COLOR_RANGE;
    return color;
}

vec3 AlignWorldToCell(vec3 position, uint cellCount)
{
    float cellSize = (VoxelGrid.Scale * 2.0) / cellCount;

    vec3 mapped = (position - VoxelGrid.Position) / cellSize;
    mapped = floor(mapped) * cellSize;

    return VoxelGrid.Position + mapped;
}

vec3 WorldToAlignedUV(vec3 pos, uint cellCount)
{
    ivec3 texel = WorldToTexel(pos, cellCount);
    return (vec3(texel) + 0.5) / cellCount;
}

float CrossCell(inout vec3 position, vec3 direction, uint cellCount)
{
    float cellSize = (VoxelGrid.Scale * 2.0) / cellCount;

    vec3 cellMin = AlignWorldToCell(position, cellCount);
    vec3 cellMax = cellMin + cellSize;

    vec3 first = (cellMin - position) / direction;
    vec3 second = (cellMax - position) / direction;

    first = max(first, second);
    float dist = min(first.x, min(first.y, first.z));

    position += direction * (dist + EPSILON);
    return dist;
}

// Adapted from two sources:
// https://www.jpgrenier.org/ssr.html
// https://seblagarde.wordpress.com/2012/09/29/image-based-lighting-approaches-and-parallax-corrected-cubemap/
RayResult Trace(Ray ray)
{
    RayResult result = RayResult(ray.Position, 0.0, vec3(0), false);

    vec3 rayABS = abs(ray.Position - VoxelGrid.Position);
    if(max(rayABS.x, max(rayABS.y, rayABS.z)) > VoxelGrid.Scale) return result;

    uint cellCount = textureSize(VoxelGrid.Color, 0).r;
    vec3 alignedUV = WorldToAlignedUV(result.Position, cellCount);

    if(textureLod(VoxelGrid.Color, alignedUV, 0.0).a > 0.5)
    {
        result.Hit = true;
        return result;
    }

    for(uint i = 0; i < VOXEL_TRACE_MAX_ITERATIONS; i++)
    {
        result.Distance += CrossCell(result.Position, ray.Direction, cellCount);

        rayABS = abs(result.Position - VoxelGrid.Position);
        if(result.Distance >= ray.MaximumDistance || max(rayABS.x, max(rayABS.y, rayABS.z)) > VoxelGrid.Scale)
            return result;

        alignedUV = WorldToAlignedUV(result.Position, cellCount);

        if(textureLod(VoxelGrid.Color, alignedUV, 0.0).a > 0.5)
        {
            result.Hit = true;
            break;
        }
    }

    return result;
}

RayResult TraceOffset(Ray ray, vec3 normal)
{
    uint cellCount = textureSize(VoxelGrid.Color, 0).r;
    float cellSize = (VoxelGrid.Scale * 2.0) / cellCount;

    ray.Position += cellSize * normal * 1.25;
    ray.Position += cellSize * ray.Direction * .5;

    return Trace(ray);
}