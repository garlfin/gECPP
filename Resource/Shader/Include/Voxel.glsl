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

uniform layout(binding = VOXEL_TEXTURE_LOCATION, r11f_g11f_b10f) restrict image3D VoxelColorOut;
uniform layout(binding = VOXEL_DATA_LOCATION, r8ui) restrict uimage3D VoxelDataOut;

struct VoxelGridData
{
    vec3 Position;
    float Scale;
    uint CellCount; // AKA Texture Size
    BINDLESS_TEXTURE(sampler3D, Color);
    BINDLESS_TEXTURE(sampler3D, Data);
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

struct Voxel
{
    vec3 Color;
    vec4 RSMA; // (Metallic, Roughness, Specular, Alpha)
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
RayResult Trace(Ray, out Voxel);
void WriteVoxel(vec3, Voxel);
Voxel ReadVoxel(vec3);
Voxel ReadVoxel(ivec3);

// Helper Functions
vec3 WorldToUV(vec3);
ivec3 UVToTexel(vec3, uint);
ivec3 WorldToTexel(vec3, uint);
vec3 AlignWorldToCell(vec3, uint);
vec3 CrossCell(uint, vec3, vec3);

// Implementation
vec3 WorldToUV(vec3 pos)
{
    vec3 uv = (pos - VoxelGrid.Position) / VoxelGrid.Scale;
    return uv * 0.5 + 0.5;
}

ivec3 UVToTexel(vec3 uv, uint cellCount)
{
    return ivec3(uv * cellCount);
}

ivec3 WorldToTexel(vec3 pos, uint cellCount)
{
    return UVToTexel(WorldToUV(pos), cellCount);
}

void WriteVoxel(vec3 pos, Voxel voxel)
{
    // Outside the grid
    // if(any(lessThan(pos, boxMin)) || any(greaterThan(pos, boxMax))) return;

    ivec3 texel = WorldToTexel(pos, VoxelGrid.CellCount);

    voxel.RSMA = clamp(voxel.RSMA, vec4(0.0), vec4(1.0));

    uint packedRSMA = 0; // 3, 3, 1, 1
    packedRSMA |= uint(voxel.RSMA.r * 8.0); // Roughness
    packedRSMA |= uint(voxel.RSMA.g * 8.0) << 3; // Specular
    packedRSMA |= uint(voxel.RSMA.b) << 6; // Metallic
    packedRSMA |= uint(voxel.RSMA.a) << 7; // Alpha (Solid)

    imageStore(VoxelColorOut, texel, vec4(voxel.Color, 1.0));
    imageStore(VoxelDataOut, texel, ivec4(packedRSMA));
}

Voxel ReadVoxel(vec3 pos)
{
    vec3 uv = WorldToUV(pos);
    ivec3 texel = UVToTexel(uv, VoxelGrid.CellCount);

    Voxel voxel;

    voxel.Color = textureLod(VoxelGrid.Color, uv, 0.0).rgb;

    uint packedRSMA = imageLoad(VoxelDataOut, texel).r;
    voxel.RSMA.r = float(packedRSMA & 7u) / 8.0;
    voxel.RSMA.g = float(packedRSMA >> 3 & 7u) / 8.0;
    voxel.RSMA.b = float(packedRSMA >> 6 & 1u);
    voxel.RSMA.a = float(packedRSMA >> 7 & 1u);

    return voxel;
}

Voxel ReadVoxel(ivec3 texel)
{
    Voxel voxel;

    voxel.Color = imageLoad(VoxelColorOut, texel).rgb;
    uint packedRSMA = imageLoad(VoxelDataOut, texel).r;

    voxel.RSMA.r = float(packedRSMA & 7u) / 8.0;
    voxel.RSMA.g = float(packedRSMA >> 3 & 7u) / 8.0;
    voxel.RSMA.b = float(packedRSMA >> 6 & 1u);
    voxel.RSMA.a = float(packedRSMA >> 7 & 1u);

    return voxel;
}

vec3 AlignWorldToCell(vec3 position, uint cellCount)
{
    float cellSize = (VoxelGrid.Scale * 2.0) / cellCount;

    vec3 mapped = (position - VoxelGrid.Position) / cellSize;
    mapped = floor(mapped) * cellSize;

    return VoxelGrid.Position + mapped;
}

vec3 CrossCell(uint cellCount, vec3 position, vec3 direction)
{
    float cellSize = (VoxelGrid.Scale * 2.0) / cellCount;

    vec3 cellMin = AlignWorldToCell(position, cellCount);
    vec3 cellMax = cellMin + cellSize;

    vec3 first = (cellMin - position) / direction;
    vec3 second = (cellMax - position) / direction;

    first = max(first, second);
    float dist = min(first.x, min(first.y, first.z));

    return position + direction * (dist + EPSILON);
}

// Adapted from two sources:
// https://www.jpgrenier.org/ssr.html
// https://seblagarde.wordpress.com/2012/09/29/image-based-lighting-approaches-and-parallax-corrected-cubemap/
RayResult Trace(Ray ray, out Voxel voxel)
{
    ray.Direction = normalize(ray.Direction);

    RayResult result = RayResult(ray.Position, 0.0, vec3(0), false);

    vec3 rayABS = abs(ray.Position - VoxelGrid.Position);
    if(max(rayABS.x, max(rayABS.y, rayABS.z)) > VoxelGrid.Scale) return result;

    // TODO: avoid self-intersection;

    for(uint i = 0; i < VOXEL_TRACE_MAX_ITERATIONS; i++)
    {
        result.Position = CrossCell(VoxelGrid.CellCount, result.Position, ray.Direction);

        voxel = ReadVoxel(result.Position);
        if(voxel.RSMA.a == 1.0) break;
    }

    return result;
}