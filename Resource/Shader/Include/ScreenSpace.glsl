#include "Camera.glsl"
#include "Voxel.glsl"
#include "Shadow.glsl"

#ifndef RAY_MAX_ITERATIONS
    #define RAY_MAX_ITERATIONS 64
#endif

#ifndef RAY_THICKNESS
    #define RAY_THICKNESS 0.1
#endif

// Functions
#ifdef EXT_BINDLESS
RayResult SS_Trace(Ray);
#endif

// Helper Functions
vec3 SS_WorldToUV(vec3);
ivec2 SS_UVToTexel(vec2, uint);
vec2 SS_TexelToUV(ivec2, uint);
vec2 SS_AlignUVToCell(vec2, uint);
float SS_CrossCell(inout vec3, vec3, uint);

// Implementation
#ifdef EXT_BINDLESS
RayResult SS_Trace(Ray ray)
{
    ray.Position = SS_WorldToUV(ray.Position);
    ray.Direction = vec3(Camera.View[0] * vec4(ray.Direction, 1.0));
    ray.Direction = normalize(ray.Direction);

    RayResult result = RayResult(ray.Position, 0.0, vec3(0.0), false);
    result.Distance += SS_CrossCell(result.Position, ray.Direction, 0);

    int size = textureSize(Camera.Depth, 0).r;
    int mipCount = textureQueryLevels(Camera.Depth) - 1;
    int mip = 0;

    for(uint i = 0; i < RAY_MAX_ITERATIONS; i++)
    {
        if(result.Distance > ray.MaximumDistance || TexcoordOutOfBounds(result.Position.xy)) break;

        ivec2 oldCell = SS_UVToTexel(result.Position.xy, mip);
        vec2 alignedUV = SS_TexelToUV(oldCell, mip);

        float depth = textureLod(Camera.Depth, alignedUV.xy, float(mip)).r;

        if(depth > result.Position.z)
        {
            if(mip == 0)
            {
                result.Hit = result.Position.z - depth < RAY_THICKNESS;
                break;
            }
            else mip--;
        }
        else
        {
            result.Distance += SS_CrossCell(result.Position, ray.Direction, mip);
            ivec2 newCell = SS_UVToTexel(result.Position.xy, size >> (mip + 1));

            if(oldCell >> 1 != newCell) mip = min(mip + 1, mipCount);
        }
    }

    return result;
}
#endif

vec3 SS_WorldToUV(vec3 pos)
{
    vec4 viewSpace = Camera.View[0] * vec4(pos, 1.0);
    vec4 projSpace = Camera.Projection * viewSpace;

    projSpace.xyz /= projSpace.w;
    projSpace.xy = projSpace.xy * 0.5 + 0.5;

    return vec3(projSpace.xy, -viewSpace.z);
}

vec2 SS_TexelToUV(ivec2 texel, uint mip)
{
    return (vec2(texel) + 0.5) / textureSize(Camera.Depth, int(mip));
}

ivec2 SS_UVToTexel(vec2 uv, uint mip)
{
    return ivec2(uv * textureSize(Camera.Depth, int(mip)));
}

vec2 SS_AlignUVToCell(vec2 uv, uint mip)
{
    return SS_TexelToUV(SS_UVToTexel(uv, mip), mip);
}

float SS_CrossCell(inout vec3 position, vec3 direction, uint mip)
{
    ivec2 size = textureSize(Camera.Depth, int(mip));
    vec2 cellSize = vec2(1.0) / size;

    vec2 cellMin = SS_AlignUVToCell(position.xy, mip);
    vec2 cellMax = cellMin + cellSize;

    vec2 first = (cellMin - position.xy) / direction.xy;
    vec2 second = (cellMax - position.xy) / direction.xy;

    first = max(first, second);
    float dist = min(first.x, first.y);

    position += direction * (dist + EPSILON);
    return dist;
}