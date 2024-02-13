#include "Camera.glsl"
#include "Voxel.glsl"
#include "Shadow.glsl"

#ifndef RAY_MAX_ITERATIONS
    #define RAY_MAX_ITERATIONS 32
#endif

// Functions
#ifdef EXT_BINDLESS
RayResult SS_Trace(Ray);
#endif

// Helper Functions
vec3 SS_WorldToUV(vec3);
vec2 SS_TexelToUV(ivec2, uint); // Mip
ivec2 SS_WorldToTexel(vec3, uint); // Mip
vec2 SS_WorldToAlignedUV(vec3, uint); // Mip
float SS_CrossCell(inout vec3, vec3, uint); // Mip

// Implementation
RayResult SS_Trace(Ray ray)
{
    RayResult result = RayResult(ray.Position, 0.0, vec3(0.0), false);

    ray.Direction = normalize(ray.Direction);
    ray.Direction *= ray.MaximumDistance / RAY_MAX_ITERATIONS;

    for(uint i = 0; i < RAY_MAX_ITERATIONS; i++)
    {
        result.Position += ray.Direction;

        vec3 uv = SS_WorldToUV(result.Position);
        float dist = uv.z - textureLod(Camera.Depth, uv.xy, 0.0).r;

        if(uv.x > 1 || uv.y > 1 || uv.x < 0 || uv.y < 0) break;
        if(dist > 0)
        {
            result.Hit = true;
            break;
        }
    }

    return result;
}

vec3 SS_WorldToUV(vec3 pos)
{
    vec4 viewSpace = Camera.View[0] * vec4(pos, 1.0);
    vec4 projSpace = Camera.Projection * viewSpace;

    projSpace.xy /= projSpace.w;
    projSpace.xy = projSpace.xy * 0.5 + 0.5;

    return vec3(projSpace.xy, viewSpace.z);
}

vec2 SS_TexelToUV(ivec2 texel, uint lod)
{
    return (vec2(texel) + 0.5) / textureSize(Camera.Color, int(lod));
}

ivec2 SS_WorldToTexel(vec3 pos, uint lod)
{
    vec2 uv = SS_WorldToUV(pos).xy;
    return ivec2(uv * textureSize(Camera.Color, int(lod)));
}

vec2 SS_WorldToAlignedUV(vec3 pos, uint lod)
{
    vec2 uv = SS_WorldToUV(pos).xy;
    ivec2 size = textureSize(Camera.Color, int(lod));

    return floor(uv * size) / size;
}

float SS_CrossCell(inout vec3, vec3, uint); // CellCount