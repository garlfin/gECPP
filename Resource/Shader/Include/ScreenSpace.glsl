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
    #define RAY_THICKNESS 0.1
#endif

#ifndef RAY_MAX_MIP
    #define RAY_MAX_MIP 5
#endif

struct AOSettings
{
    float Radius;
    float RadiusPixels;
    float InvRadiusS;
};

// Functions
#ifdef EXT_BINDLESS
RayResult SS_Trace(Ray);
float SS_AO(AOSettings, Vertex);
#endif

// Helper Functions
vec3 SS_WorldToUV(vec3);
ivec2 SS_UVToTexel(vec2, ivec2);
vec2 SS_TexelToUV(ivec2, ivec2);
vec2 SS_AlignUVToCell(vec2, ivec2);
float SS_CrossCell(inout vec3, vec3, ivec2);
vec3 SS_DirToView(vec3);
float LengthSquared(vec3 v) { return dot(v, v); }

// Implementation
#ifdef EXT_BINDLESS
float SS_CrossCell(inout vec3 pos, vec3 dir, ivec2 size)
{
    vec2 cellSize = 1.0 / size;
    vec2 cellMin = SS_AlignUVToCell(pos.xy, size);
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

vec3 SS_DirToView(vec3 dir)
{
    return normalize(vec3(Camera.View[0] * vec4(dir, 0.0))) * vec3(1, 1, -1);
}

vec2 SS_TexelToUV(ivec2 texel, ivec2 size)
{
    return (vec2(texel) + 0.5) / size;
}

ivec2 SS_UVToTexel(vec2 uv, ivec2 size)
{
    return ivec2(uv * size);
}

vec2 SS_AlignUVToCell(vec2 uv, ivec2 size)
{
    return floor(uv * size) / size;
}

RayResult SS_Trace(Ray ray)
{
    vec3 rayStart = SS_WorldToUV(ray.Position);
    vec3 rayEnd = SS_WorldToUV(ray.Position + ray.Direction * ray.MaximumDistance);
    vec3 rayDir = rayEnd - rayStart;

    float rayLength = length(rayDir.xy);
    float near = rayStart.z, far = rayEnd.z;

    int mip = 0, maxMip = textureQueryLevels(Camera.Depth) - 1;

    #ifdef RAY_MAX_MIP
        maxMip = min(maxMip, RAY_MAX_MIP);
    #endif

    ivec2 texSize = textureSize(Camera.Depth, 0);

    RayResult result = RayResult(rayStart, 0.f, vec3(0.f), false);

    if(rayDir.z < 0.0) return result;
    SS_CrossCell(result.Position, rayDir, texSize);
    for(int i = 0; i < RAY_MAX_ITERATIONS; i++)
    {
        float lerp = distance(rayStart.xy, result.Position.xy) / rayLength;

        vec3 uv = result.Position;
        uv.z = (near * far) / (near * lerp + far * (1 - lerp));

        if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) break;

        float depth = textureLod(Camera.Depth, uv.xy, float(mip)).r;

        if(depth + 0.01 < uv.z)
            if(mip == 0) { result.Hit = uv.z - depth < RAY_THICKNESS; break; }
            else mip--;
        else
        {
            ivec2 cell = SS_UVToTexel(result.Position.xy, texSize >> (mip + 1));
            SS_CrossCell(result.Position, rayDir, texSize >> mip);
            ivec2 newCell = SS_UVToTexel(result.Position.xy, texSize >> (mip + 1));

            if(cell != newCell) mip++;
            mip = min(mip, maxMip);
        }
    }

    return result;
}

float SS_ComputeAO(AOSettings s, Vertex vert)
{
    return 0.0;
}

float SS_AO(AOSettings s, Vertex vert)
{
    vert.Position = SS_WorldToUV(vert.Position);
    vert.Normal = SS_DirToView(vert.Normal);
    float depth = vert.Position.z;

    s.RadiusPixels = s.Radius * 0.5 / tan(Camera.Parameters.x * 0.5) * Camera.Size.y;
    s.InvRadiusS = -1.0 / (s.RadiusPixels * s.RadiusPixels);

    s.RadiusPixels /= depth;

    if(s.RadiusPixels < 1.0) return 1.0;

    float ao = SS_ComputeAO(s, vert);
    return clamp(1.0 - ao * 2.0, 0.0, 1.0);
}
#endif