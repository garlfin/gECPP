#include "Camera.glsl"
#include "Voxel.glsl"
#include "Noise.glsl"
#include "Math.glsl"

#ifndef RAY_MAX_ITERATIONS
    #define RAY_MAX_ITERATIONS 128
#endif

#ifndef RAY_THICKNESS
    #define RAY_THICKNESS 0.2
#endif

#ifndef RAY_CELL_EPSILON
    #define RAY_CELL_EPSILON 0.1
#endif

#ifndef RAY_EPSILON
    #define RAY_EPSILON 0.01
#endif

#ifndef RAY_MAX_BIAS
    #define RAY_MAX_BIAS 0.1
#endif

#ifndef RAY_MIN_MIP
    #define RAY_MIN_MIP 0
#endif

#ifndef RAY_MIP_SKIP
    #define RAY_MIP_SKIP 1
#endif

#define RAY_MAX_MIP 4

struct AOSettings
{
    float Radius;
    float RadiusPixels;
    float InvRadiusS;
};

struct LinearRaySettings
{
    int Iterations;
    float NormalBias;
    float RayBias;
    vec3 Normal;
    float SearchBias;
};

// Functions
#if defined(EXT_BINDLESS) && defined(FRAGMENT_SHADER)
RayResult SS_Trace(Ray, out int);
RayResult SS_TraceRough(Ray, LinearRaySettings);
float SS_AO(AOSettings, Vertex);
#endif

// Helper Functions
vec3 SS_WorldToUV(vec3);
ivec2 SS_UVToTexel(vec2, ivec2);
vec2 SS_TexelToUV(ivec2, ivec2);
vec2 SS_AlignUVToTexel(vec2, ivec2);
void SS_CrossCell(inout vec3, vec3, ivec2);
vec3 SS_DirToView(vec3);
float LengthSquared(vec3 v) { return dot(v, v); }
float SS_GetDepthWithBias(float d, float bias) { return d + min(RAY_MAX_BIAS, d * bias); }
float SS_Sign(float f) { return f < 0 ? -1 : 1; }
vec2 SS_Sign(vec2 v) { return vec2(SS_Sign(v.x), SS_Sign(v.y)); }
float SS_CorrectDepth(vec2, float);
float SS_CorrectDepth(float near, float far, float lerp) { return SS_CorrectDepth(vec2(near, far), lerp); }

// Implementation
#if defined(EXT_BINDLESS) && defined(FRAGMENT_SHADER)
void SS_CrossCell(inout vec3 pos, vec3 dir, ivec2 size)
{
    vec2 cellSize = 0.5 / size;

    vec2 planes = SS_AlignUVToTexel(pos.xy, size);
    planes += SS_Sign(dir.xy) * cellSize * (1 + EPSILON);

    vec2 solution = (planes - pos.xy) / dir.xy;
    float dist = min(solution.x, solution.y);

    pos += dist * dir;
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

// Snaps to center of texel
vec2 SS_AlignUVToTexel(vec2 uv, ivec2 size)
{
    return SS_TexelToUV(SS_UVToTexel(uv, size), size);
}

float SS_CorrectDepth(vec2 planes, float lerp)
{
    return (planes.x * planes.y) / (planes.x * lerp + planes.y * (1.0 - lerp));
}

RayResult SS_Trace(Ray ray)
{
    ray.Direction = normalize(ray.Direction);

    vec3 rayStart = SS_WorldToUV(ray.Position);
    vec3 rayEnd = SS_WorldToUV(ray.Position + ray.Direction * ray.Length);
    vec3 rayDir = rayEnd - rayStart;

    float rayLength = length(rayDir.xy);
    float near = rayStart.z, far = rayEnd.z;

    int mip = RAY_MIN_MIP;
    int maxMip = textureQueryLevels(Camera.Depth) - 1;

#ifdef RAY_MAX_MIP
    maxMip = min(maxMip, RAY_MAX_MIP);
#endif

    RayResult result = RayResult(rayStart, 0.0, vec3(0.0), RAY_RESULT_NO_HIT);

    if(far < near)
    {
        result.Result = RAY_RESULT_OUT_OF_BOUNDS;
        return result; // Temporary while I debug backwards rays.
    }

    int i;
    for(i = 0; i < RAY_MAX_ITERATIONS; i++)
    {
        vec3 oldPos = result.Position;
        ivec2 mipSize = textureSize(Camera.Depth, mip);
        ivec2 mipSizeUp = textureSize(Camera.Depth, mip + RAY_MIP_SKIP);
        ivec2 oldCell = SS_UVToTexel(result.Position.xy, mipSizeUp);

        SS_CrossCell(result.Position, rayDir, mipSize);

        ivec2 newCell = SS_UVToTexel(result.Position.xy, mipSizeUp);
        float lerp = distance(rayStart.xy, result.Position.xy) / rayLength;

        vec3 uv = result.Position;
        uv.z = SS_CorrectDepth(near, far, lerp);

        if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
        {
            result.Result = RAY_RESULT_OUT_OF_BOUNDS;
            break;
        }

        float depth = textureLod(Camera.Depth, uv.xy, float(mip)).r;
        depth = SS_GetDepthWithBias(depth, 0.01);

        if(depth < uv.z)
            if(mip == RAY_MIN_MIP)
            {
                bool crossed = uv.z < depth + RAY_THICKNESS;
                result.Result = crossed ? RAY_RESULT_HIT : RAY_RESULT_TOO_FAR;
                break;
            }
            else
            {
                mip--;
                result.Position = oldPos;
            }
        else if(oldCell != newCell)
            mip = min(mip + RAY_MIP_SKIP, maxMip);
    }

    if(i == RAY_MAX_ITERATIONS) result.Result = RAY_RESULT_EXHAUSTED;
    return result;
}

RayResult SS_TraceRough(Ray ray, LinearRaySettings settings)
{
    ray.Direction = normalize(ray.Direction);

    float searchBias;

    if(settings.SearchBias < 0.0)
        searchBias = 1.0 / -settings.SearchBias;
    else
        searchBias = 1.0 + settings.SearchBias;

    ray.Position += settings.Normal * settings.NormalBias;
    ray.Position += ray.Direction * settings.RayBias * (1.0 + max(IGNSample, EPSILON)) / settings.Iterations;
    ray.Direction *= ray.Length;

    vec3 rayStart = SS_WorldToUV(ray.Position);
    vec3 rayEnd = SS_WorldToUV(ray.Position + ray.Direction);

    vec3 rayDir = (rayEnd - rayStart) / settings.Iterations;

    float near = rayStart.z, far = rayEnd.z;

    RayResult result = RayResult(rayStart, 0.0, vec3(0.0), RAY_RESULT_NO_HIT);

    int i;
    for(i = 0; i < settings.Iterations; i++)
    {
        float lerp = pow(float(i) / settings.Iterations, searchBias);

        vec3 uv = result.Position = mix(rayStart, rayEnd, lerp);
        uv.z = SS_CorrectDepth(near, far, lerp);

        if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || uv.z < 0.0) break;

        float depth = textureLod(Camera.Depth, uv.xy, 0.f).r;
        depth = SS_GetDepthWithBias(depth, 0.05);

        if(depth < uv.z)
        {
            result.Result = uv.z - depth < RAY_THICKNESS ? RAY_RESULT_HIT : RAY_RESULT_TOO_FAR;
            return result;
        }
    }

    if(i == RAY_MAX_ITERATIONS) result.Result = RAY_RESULT_EXHAUSTED;
    return result;
}

float SS_ComputeAO(AOSettings s, Vertex vert)
{
    return 0.5;
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
    return clamp(ao * 2.0 - 1.0, 0.0, 1.0);
}
#endif