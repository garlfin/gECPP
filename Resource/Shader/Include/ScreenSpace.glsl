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
ivec2 SS_UVToTexel(vec2, int);
vec2 SS_TexelToUV(ivec2, int);
vec2 SS_AlignUVToCell(vec2, int);
float SS_CrossCell(inout vec3, vec3, int);

// Implementation
#ifdef EXT_BINDLESS
RayResult SS_Trace(Ray ray)
{
    ray.Position = SS_WorldToUV(ray.Position);
    ray.Direction = vec3(Camera.View[0] * vec4(ray.Direction, 0.0)) * vec3(1, 1, -1);

    RayResult result = RayResult(ray.Position, 0.f, ray.Direction, false);
    if(ray.Direction.z < EPSILON) return result;

    int mip = 0;
    int mipCount = textureQueryLevels(Camera.Depth);

    result.Distance += SS_CrossCell(result.Position, ray.Direction, 0);

    for(int i = 0; i < RAY_MAX_ITERATIONS; i++)
    {
        vec3 uv = result.Position;
        ivec2 cell = SS_UVToTexel(uv.xy, mip + 1);
        if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) break;

        float depth = texture(Camera.Depth, uv.xy, float(mip)).r;
        float offset = uv.z - depth;

        if(offset > 0.0)
        {
            result.Position -= offset / ray.Direction.z * ray.Direction;

            if(mip == 0)
            {
                result.Hit = true;
                break;
            } else mip--;
        }
        else
        {
            result.Distance += SS_CrossCell(result.Position, ray.Direction, mip);
            ivec2 newCell = SS_UVToTexel(result.Position.xy, mip + 1);

            if(cell != newCell) if(mip == mipCount - 1) break; else mip++;
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

    return vec3(projSpace.xy, -viewSpace.z);
}

vec2 SS_TexelToUV(ivec2 texel, int mip)
{
    return (vec2(texel) + 0.5) / textureSize(Camera.Depth, mip);
}

ivec2 SS_UVToTexel(vec2 uv, int mip)
{
    return ivec2(uv * textureSize(Camera.Depth,mip) );
}

vec2 SS_AlignUVToCell(vec2 uv, int mip)
{
    return SS_TexelToUV(SS_UVToTexel(uv, mip), mip);
}

float SS_CrossCell(inout vec3 position, vec3 direction, int mip)
{
    ivec2 texSize = textureSize(Camera.Depth, mip);
    vec2 cellMin = floor(position.xy * texSize) / texSize;
    vec2 cellMax = ceil(position.xy * texSize) / texSize;

    vec2 first = (cellMin - position.xy) / direction.xy;
    vec2 second = (cellMax - position.xy) / direction.xy;

    first = max(first, second);
    float dist = min(first.x, first.y);

    position += direction * (dist + EPSILON);
    return dist;
}
#endif