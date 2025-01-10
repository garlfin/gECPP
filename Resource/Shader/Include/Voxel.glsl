#include "Bindless.glsl"
#include "Math.glsl"
#include "SphericalHarmonics.glsl"

#ifndef VOXEL_TEXTURE_LOCATION
    #define VOXEL_TEXTURE_LOCATION 0
#endif

#ifndef VOXEL_DATA_LOCATION
    #define VOXEL_DATA_LOCATION 1
#endif

#ifndef VOXEL_TRACE_MAX_ITERATIONS
    #define VOXEL_TRACE_MAX_ITERATIONS 64
#endif

#ifndef VOXEL_CONE_TRACE_MAX_ITERATIONS
    #define VOXEL_CONE_TRACE_MAX_ITERATIONS 64
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

#define VOXEL_GRID_RESOLUTION 8
#define VOXEL_MAX_PROBES 64
#define VOXEL_SAMPLE_DIRECTIONS 6

uniform layout(binding = VOXEL_TEXTURE_LOCATION, PIPELINE_COLOR_FORMAT) restrict image3D VoxelColorOut;

struct VoxelGridData
{
    vec3 Position;
    float Scale;
    BINDLESS_TEXTURE(sampler3D, Color);
    BINDLESS_TEXTURE(samplerCubeArray, ProbeColor);
};

#if defined(FRAGMENT_SHADER) && !defined(GL_ARB_bindless_texture)
uniform sampler3D VoxelColor;
uniform sampler3D VoxelData;
#endif

#ifndef VOXEL_UNIFORM_LOCATION
    #define VOXEL_UNIFORM_LOCATION 4
#endif

layout(std430, binding = VOXEL_UNIFORM_LOCATION) restrict buffer VoxelGridUniform
{
    VoxelGridData VoxelGrid;
};

// Functions
#ifdef EXT_BINDLESS
RayResult Voxel_Trace(Ray);
RayResult Voxel_TraceOffset(Ray, vec3);
RayResult Voxel_Trace(Ray, float);
RayResult Voxel_TraceOffset(Ray, float, vec3);
#endif

// Helper Functions
vec3 Voxel_WorldToUV(vec3);
ivec3 Voxel_UVToTexel(vec3, uint);
ivec3 Voxel_WorldToTexel(vec3, uint);
vec3 Voxel_TexelToUV(ivec3, uint);
vec3 Voxel_AlignUVToTexel(vec3, uint);
vec3 Voxel_AlignWorldToTexel(vec3, uint);
float Voxel_CrossCell(inout vec3, vec3, uint, float);
vec4 Voxel_PackColor(vec4);
vec3 Voxel_PackColor(vec3);
vec4 Voxel_UnpackColor(vec4);
vec3 Voxel_UnpackColor(vec3);

// Implementation
vec3 Voxel_WorldToUV(vec3 pos)
{
    vec3 uv = (pos - VoxelGrid.Position) / VoxelGrid.Scale;
    return uv * 0.5 + 0.5;
}

ivec3 Voxel_UVToTexel(vec3 uv, uint cellCount)
{
    return ivec3(uv * cellCount);
}

ivec3 Voxel_WorldToTexel(vec3 pos, uint cellCount)
{
    return Voxel_UVToTexel(Voxel_WorldToUV(pos), cellCount);
}

vec3 Voxel_TexelToUV(ivec3 texel, uint cellCount)
{
    return (vec3(texel) + 0.5) / cellCount;
}

vec3 Voxel_AlignUVToTexel(vec3 pos, uint cellCount)
{
    return Voxel_TexelToUV(Voxel_UVToTexel(pos, cellCount), cellCount);
}

vec3 Voxel_AlignWorldToTexel(vec3 pos, uint cellCount)
{
    float cellSize = (VoxelGrid.Scale * 2.0) / cellCount;

    vec3 mapped = (pos - VoxelGrid.Position) / cellSize;
    mapped = (floor(mapped) + 0.5) * cellSize;

    return VoxelGrid.Position + mapped;
}

vec3 Voxel_PackColor(vec3 color)
{
    return pow(color / VOXEL_COLOR_RANGE, vec3(1.0 / 2.2));
}

vec4 Voxel_PackColor(vec4 color)
{
    color.rgb = pow(color.rgb / VOXEL_COLOR_RANGE, vec3(1.0 / 2.2));
    return color;
}

vec3 Voxel_UnpackColor(vec3 color)
{
    return pow(color, vec3(2.2)) * VOXEL_COLOR_RANGE;
}

uint Voxel_PackColorUInt(vec3 color)
{
    color = saturate(color);

    uint result;
    result = uint(color.r * 1023.0) & 1023;
    result |= (uint(color.g * 1023.0) & 1023) << 10;
    result |= (uint(color.b * 1023.0) & 1023) << 20;
    return result;
}

vec3 Voxel_UnpackColorUInt(uint color)
{
    vec3 result;
    result.r = (color & 1023) / 1023.0;
    result.g = (color >> 10 & 1023) / 1023.0;
    result.b = (color >> 20 & 1023) / 1023.0;
    return result;
}

vec4 Voxel_UnpackColor(vec4 color)
{
    color.rgb = pow(color.rgb, vec3(2.2)) * VOXEL_COLOR_RANGE;
    return color;
}

float Voxel_CrossCell(inout vec3 pos, vec3 dir, uint cellCount, float crossDirection)
{
    float halfCellSize = VoxelGrid.Scale / cellCount;

    vec3 planes = Voxel_AlignWorldToTexel(pos, cellCount);
    planes += Sign(dir) * halfCellSize * (1 + EPSILON * crossDirection);

    vec3 solution = (planes - pos) / dir;
    float dist = min(solution.x, min(solution.y, solution.z));

    pos += dist * dir;
    return dist;
}

// Adapted from two sources:
// https://www.jpgrenier.org/ssr.html
// https://seblagarde.wordpress.com/2012/09/29/image-based-lighting-approaches-and-parallax-corrected-cubemap/
#ifdef EXT_BINDLESS
RayResult Voxel_Trace(Ray ray)
{
    RayResult result = RayResult(ray.Position, 0.0, vec3(0), RAY_RESULT_NO_HIT);
    int size = textureSize(VoxelGrid.Color, 0).r;
    int mipCount = textureQueryLevels(VoxelGrid.Color);
    int mip = ray.BaseMip;

    for(uint i = 0; i < VOXEL_TRACE_MAX_ITERATIONS; i++)
    {
        vec3 rayABS = abs(result.Position - VoxelGrid.Position);
        if(result.Distance > ray.Length) break;
        if(max(rayABS.x, max(rayABS.y, rayABS.z)) > VoxelGrid.Scale) break;

        ivec3 cell = Voxel_WorldToTexel(result.Position, size >> mip);
        vec3 uv = Voxel_TexelToUV(cell, size >> mip);
        float solid = textureLod(VoxelGrid.Color, uv, float(mip)).a;
        if(solid >= 0.5)
            if(mip == ray.BaseMip)
            {
                result.Result = RAY_RESULT_HIT;
                break;
            }
            else mip--;
        else
        {
            result.Distance += Voxel_CrossCell(result.Position, ray.Direction, size >> mip, 1.0);
            ivec3 newCell = Voxel_WorldToTexel(result.Position, size >> (mip + 1));

            if (cell >> 1 != newCell)
                if (mip == mipCount - 1)
                    break;
                else
                    mip++;
        }
    }

    return result;
}

RayResult Voxel_TraceOffset(Ray ray, vec3 normal)
{
    uint cellCount = textureSize(VoxelGrid.Color, 0).r;
    float cellSize = (VoxelGrid.Scale * 2.0) / cellCount;

    ray.Position += cellSize * normal * 1.25 * (ray.BaseMip + 1);
    ray.Position += cellSize * ray.Direction * 0.5 * (ray.BaseMip + 1);

    return Voxel_Trace(ray);
}

RayResult Voxel_Trace(Ray cone, float coneAngle)
{
    RayResult result = RayResult(cone.Position, EPSILON, vec3(0), RAY_RESULT_NO_HIT);

    int size = textureSize(VoxelGrid.Color, 0).r;
    int mipCount = textureQueryLevels(VoxelGrid.Color);
    int mip = 0;

    float cellSize = 2.0 * VoxelGrid.Scale;// / size;

    for(uint i = 0; i < VOXEL_CONE_TRACE_MAX_ITERATIONS; i++)
    {
        vec3 rayABS = abs(result.Position - VoxelGrid.Position);
        if(result.Distance > cone.Length) break;
        if(max(rayABS.x, max(rayABS.y, rayABS.z)) > VoxelGrid.Scale) break;

        float coneDiameter = 2.0 * tan(coneAngle) * result.Distance;
        mip = int(log2(coneDiameter / cellSize)) + cone.BaseMip;
        int mipSize = size >> mip;

        ivec3 cell = Voxel_WorldToTexel(result.Position, mipSize);
        vec3 uv = Voxel_TexelToUV(cell, mipSize);

        float solid = textureLod(VoxelGrid.Color, uv, float(mip)).a;
        if(solid >= 0.5)
        {
            result.Result = RAY_RESULT_HIT;
            break;
        }

        result.Distance += Voxel_CrossCell(result.Position, cone.Direction, mipSize, 1.0);
    }

    return result;
}

RayResult Voxel_TraceOffset(Ray cone, float coneAngle, vec3 normal)
{
    uint cellCount = textureSize(VoxelGrid.Color, 0).r;
    float cellSize = (VoxelGrid.Scale * 2.0) / cellCount;

    cone.Position += cellSize * normal * 1.25 * (cone.BaseMip + 1);
    cone.Position += cellSize * cone.Direction * 0.5 * (cone.BaseMip + 1);

    return Voxel_Trace(cone, coneAngle);
}
#endif