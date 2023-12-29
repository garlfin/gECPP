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
    BINDLESS_TEXTURE(usampler3D, Data);
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
RayResult Trace(Ray);
RayResult TraceOffset(Ray, vec3);

void WriteVoxel(vec3, Voxel);
void WriteVoxel(ivec3, Voxel);

Voxel ReadVoxel(vec3, uint); // LOD
Voxel ReadVoxel(ivec3, uint); // LOD
Voxel ReadVoxelUV(vec3, uint); // LOD

vec4 ReadVoxelData(vec3, uint); // LOD
vec4 ReadVoxelData(ivec3, uint); // LOD
vec4 ReadVoxelDataUV(vec3, uint); // LOD


// Helper Functions
vec3 WorldToUV(vec3);
vec3 TexelToUV(vec3, uint); // CellCount
ivec3 UVToTexel(vec3, uint); // CellCount
ivec3 WorldToTexel(vec3, uint); // CellCount

vec3 AlignWorldToCell(vec3, uint); // CellCount
vec3 CrossCell(vec3, vec3, uint); // CellCount

// Implementation
vec3 WorldToUV(vec3 pos)
{
    vec3 uv = (pos - VoxelGrid.Position) / VoxelGrid.Scale;
    return uv * 0.5 + 0.5;
}

vec3 TexelToUV(ivec3 texel, uint cellCount) { return texel * ((VoxelGrid.Scale * 2.0) / cellCount); }
ivec3 UVToTexel(vec3 uv, uint cellCount) { return ivec3(uv * cellCount); }
ivec3 WorldToTexel(vec3 pos, uint cellCount) { return UVToTexel(WorldToUV(pos), cellCount); }

void WriteVoxel(ivec3 texel, Voxel voxel)
{
    voxel.RSMA = clamp(voxel.RSMA, vec4(0.0), vec4(1.0));

    uint packedRSMA = 0; // 3, 3, 1, 1
    packedRSMA |= uint(voxel.RSMA.r * 8.0); // Roughness
    packedRSMA |= uint(voxel.RSMA.g * 8.0) << 3; // Specular
    packedRSMA |= uint(voxel.RSMA.b) << 6; // Metallic
    packedRSMA |= uint(voxel.RSMA.a) << 7; // Alpha (Solid)

    imageStore(VoxelColorOut, texel, vec4(voxel.Color, 1.0));
    imageStore(VoxelDataOut, texel, ivec4(packedRSMA));
}

void WriteVoxel(vec3 pos, Voxel voxel) { WriteVoxel(WorldToTexel(pos, VoxelGrid.CellCount), voxel); }

Voxel ReadVoxel(vec3 pos, uint lod) { return ReadVoxelUV(WorldToUV(pos), lod); }
Voxel ReadVoxel(ivec3 texel, uint lod) { return ReadVoxelUV(TexelToUV(texel, VoxelGrid.CellCount >> lod), lod); }

Voxel ReadVoxelUV(vec3 uv, uint lod)
{
    Voxel voxel;

    voxel.Color = textureLod(VoxelGrid.Color, uv, float(lod)).rgb;
    voxel.RSMA = ReadVoxelDataUV(uv, lod);

    return voxel;
}

vec4 ReadVoxelDataUV(vec3 uv, uint lod)
{
    vec4 unpackedRSMA;
    uint packedRSMA = textureLod(VoxelGrid.Data, uv, float(lod)).r;

    unpackedRSMA.r = float(packedRSMA & 7u) / 8.0;
    unpackedRSMA.g = float(packedRSMA >> 3 & 7u) / 8.0;
    unpackedRSMA.b = float(packedRSMA >> 6 & 1u);
    unpackedRSMA.a = float(packedRSMA >> 7 & 1u);

    return unpackedRSMA;
}

vec4 ReadVoxelData(vec3 pos, uint lod) { return ReadVoxelDataUV(WorldToUV(pos), lod); }
vec4 ReadVoxelData(ivec3 texel, uint lod) { return ReadVoxelDataUV(TexelToUV(texel, VoxelGrid.CellCount >> lod), lod); }

vec3 AlignWorldToCell(vec3 position, uint cellCount)
{
    float cellSize = (VoxelGrid.Scale * 2.0) / cellCount;

    vec3 mapped = (position - VoxelGrid.Position) / cellSize;
    mapped = floor(mapped) * cellSize;

    return VoxelGrid.Position + mapped;
}

vec3 CrossCell(vec3 position, vec3 direction, uint cellCount)
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
RayResult Trace(Ray ray)
{
    ray.Direction = normalize(ray.Direction);

    RayResult result = RayResult(ray.Position, 0.0, vec3(0), false);

    vec3 rayABS = abs(ray.Position - VoxelGrid.Position);
    if(max(rayABS.x, max(rayABS.y, rayABS.z)) > VoxelGrid.Scale) return result;

    for(uint i = 0; i < VOXEL_TRACE_MAX_ITERATIONS; i++)
    {
        result.Position = CrossCell(result.Position, ray.Direction, VoxelGrid.CellCount);
        if(ReadVoxelData(result.Position, 0).a != 1.0) continue;

        result.Hit = true;
        break;
    }

    return result;
}

RayResult TraceOffset(Ray ray, vec3 normal)
{
    return Trace(ray);
}