#include "Camera.glsl"
#include "Voxel.glsl"
#include "Shadow.glsl"

#ifndef RAY_MAX_ITERATIONS
    #define RAY_MAX_ITERATIONS 64
#endif

#ifndef RAY_MAX_REFINE_ITERATIONS
    #define RAY_MAX_REFINE_ITERATIONS 8
#endif

#ifndef RAY_THICKNESS
    #define RAY_THICKNESS 0.5
#endif

#ifndef RAY_REFINE_THICKNESS
    #define RAY_REFINE_THICKNESS 0.3
#endif

// Functions
#ifdef EXT_BINDLESS
RayResult SS_Trace(Ray);
#endif

// Helper Functions
vec3 SS_WorldToUV(vec3);
ivec2 SS_UVToTexel(vec2, int);
vec2 SS_TexelToUV(ivec2, int);
vec2 SS_AlignUVToCell(vec2, int);
float SS_CrossCell(inout vec3, vec3, int);

// Implementation
#ifdef EXT_BINDLESS
float SS_CrossCell(inout vec3 pos, vec3 dir, int mip)
{
    vec2 size = textureSize(Camera.Depth, mip);
    vec2 cellSize = 1.0 / size;
    vec2 cellMin = SS_AlignUVToCell(pos.xy, mip);
    vec2 cellMax = cellMin + cellSize;

    vec2 first = (cellMin - pos.xy) / dir.xy;
    vec2 second = (cellMax - pos.xy) / dir.xy;

    first = max(first, second);
    float dist = min(first.x, first.y) + 0.1 * min(cellSize.x, cellSize.y);

    pos += dist * dir;
    return dist;
}

vec3 SS_WorldToUV(vec3 pos)
{
    vec4 viewSpace = Camera.View[0] * vec4(pos, 1.0);
    vec4 projSpace = Camera.Projection * vec4(viewSpace.xyz, 1.0);

    projSpace.xy /= projSpace.w;
    projSpace.xy = projSpace.xy * 0.5 + 0.5;

    return vec3(projSpace.xy, -viewSpace.z);
}

vec2 SS_TexelToUV(ivec2 texel, int mip)
{
    return (vec2(texel) + 0.5) / textureSize(Camera.Depth, mip);
}

ivec2 SS_UVToTexel(vec2 uv, int mip)
{
    return ivec2(uv * textureSize(Camera.Depth, mip));
}

vec2 SS_AlignUVToCell(vec2 uv, int mip)
{
    vec2 size = textureSize(Camera.Depth, mip);
    return floor(uv * size) / size;
}

bool SS_Refine(inout vec3 pos, vec3 dir, float near, float far, int index)
{
    bool hit = false;

    float mult = 0.5;
    float dist = float(index) - 0.5;
    pos -= dir * 0.5;

    for(int i = 0; i < RAY_MAX_REFINE_ITERATIONS; i++)
    {
        vec3 uv = pos;
        uv.z = (near * far) / mix(far, near, dist / RAY_MAX_ITERATIONS);

        float depth = textureLod(Camera.Depth, uv.xy, 0.f).r;
        float offset = uv.z - depth;

        mult *= 0.5;
        pos += sign(offset) * dir * mult;
        dist += sign(offset) * mult;
        hit = abs(offset) < RAY_REFINE_THICKNESS;
    }

    return hit;
}

RayResult SS_Trace(Ray ray)
{
    vec3 rayStart = SS_WorldToUV(ray.Position);
    vec3 rayEnd = SS_WorldToUV(ray.Position + ray.Direction * ray.MaximumDistance);
    vec3 rayDir = rayEnd - rayStart;

    float near = rayStart.z, far = rayEnd.z;
    float iterLength = ray.MaximumDistance / RAY_MAX_ITERATIONS;

    RayResult result = RayResult(rayStart, 0.f, vec3(0.f), false);
    
    if(rayDir.z <= 0.0) return result;
    rayDir /= RAY_MAX_ITERATIONS;

    for(int i = 0; i < RAY_MAX_ITERATIONS; i++)
    {
        result.Position += rayDir;

        vec3 uv = result.Position;
        uv.z = (near * far) / mix(far, near, float(i + 1) / RAY_MAX_ITERATIONS);

        float depth = textureLod(Camera.Depth, uv.xy, 0.f).r;
        float offset = uv.z - depth;

        if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) break;
        if(offset < 0.0) continue;

        result.Hit = offset < RAY_THICKNESS;
        result.Distance = float(i + 1) / RAY_MAX_ITERATIONS * ray.MaximumDistance;
        if(result.Hit) result.Hit = SS_Refine(result.Position, rayDir, near, far, i);

        break;
    }

    return result;
}
#endif