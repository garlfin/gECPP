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
    RayResult result = RayResult(ray.Position, 0.f, vec3(0.f), false);

    for(int i = 0; i < RAY_MAX_ITERATIONS; i++)
    {
        result.Position += ray.Direction * 0.1;

        vec3 viewPos = SS_WorldToUV(result.Position);
        if(viewPos.x < 0.f || viewPos.x > 1.f || viewPos.y < 0.f || viewPos.y > 1.f) break;

        float depth = textureLod(Camera.Depth, viewPos.xy, 0.f).a;
        if(viewPos.z < depth)
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
#endif