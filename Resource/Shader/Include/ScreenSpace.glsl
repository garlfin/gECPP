#include "Camera.glsl"
#include "Noise.glsl"
#include "Math.glsl"

#ifndef RAY_EPSILON
    #define RAY_EPSILON 0.01
#endif

#ifndef RAY_MAX_BIAS
    #define RAY_MAX_BIAS 0.1
#endif

#ifndef RAY_MIP_SKIP
    #define RAY_MIP_SKIP 1
#endif

#ifndef SS_BIAS
    #define SS_BIAS 0.01
#endif

#define RAY_MAX_MIP 4

struct AOSettings
{
    uint Iterations;
    float Radius;
    float Thickness;
    float Intensity;
};

struct AOSettingsInternal
{
    AOSettings _base;
    float RadiusPixels;
    float InvRadiusS;
};

struct SSRay
{
    vec3 Start;
    vec3 End;
    int BaseMip;
};

struct SSRaySettings
{
    int Iterations;
    float NormalBias;
    float Thickness;
    vec3 Normal;
};

struct SSLinearRaySettings
{
    SSRaySettings Base;
    float RayBias;
    float SearchBias;
};

// Functions
#if defined(EXT_BINDLESS) && defined(FRAGMENT_SHADER)
RayResult SS_Trace(SSRay, SSRaySettings);
RayResult SS_TraceRough(SSRay, SSLinearRaySettings);
SSRay CreateSSRayHiZ(Ray, SSRaySettings);
SSRay CreateSSRayLinear(Ray, SSLinearRaySettings);
float SS_AO(AOSettings, Vertex);
#endif


// Helper Functions
vec3 SS_WorldToUV(vec3);
vec3 SS_WorldToView(vec3);
vec3 SS_ViewToUV(vec3);
ivec2 SS_UVToTexel(vec2, ivec2);
vec2 SS_TexelToUV(ivec2, ivec2);
vec2 SS_AlignUVToTexel(vec2, ivec2);
vec3 SS_CrossCell(vec3, vec3, ivec2, float);
vec3 SS_DirToView(vec3);
float LengthSquared(vec3 v) { return dot(v, v); }
float SS_GetDepthWithBias(float d, float bias) { return d + min(RAY_MAX_BIAS, d * bias); }
float SS_CorrectDepth(vec2, float);
float SS_CorrectDepth(float near, float far, float lerp) { return SS_CorrectDepth(vec2(near, far), lerp); }
float SS_GetAOStrength(float ao, float strength);

// Implementation
#if defined(EXT_BINDLESS) && defined(FRAGMENT_SHADER)
vec3 SS_CrossCell(vec3 pos, vec3 dir, ivec2 size, float crossDirection)
{
    vec2 halfCellSize = 0.5 / size;

    vec2 planes = SS_AlignUVToTexel(pos.xy, size);
    planes += Sign(dir.xy) * halfCellSize * (1 + EPSILON * crossDirection);

    vec2 solution = (planes - pos.xy) / dir.xy;
    float dist = min(solution.x, solution.y);

    return pos + dist * dir;
}

vec3 SS_WorldToView(vec3 worldSpace)
{
    vec3 viewSpace = vec3(Camera.View[0] * vec4(worldSpace, 1.0));
    return vec3(viewSpace.xy, -viewSpace.z);
}

vec3 SS_ViewToUV(vec3 viewSpace)
{
    viewSpace *= vec3(1, 1, -1);
    vec4 projSpace = Camera.Projection * vec4(viewSpace.xyz, 1.0);

    projSpace.xy /= projSpace.w;
    projSpace.xy = projSpace.xy * 0.5 + 0.5;

    return vec3(projSpace.xy, -viewSpace.z);
}

vec3 SS_WorldToUV(vec3 worldSpace)
{
    vec3 viewSpace = SS_WorldToView(worldSpace);
    return SS_ViewToUV(viewSpace);
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

RayResult SS_Trace(SSRay ray, SSRaySettings settings)
{
    float thickness = settings.Thickness > 0 ? settings.Thickness : FLT_INF;
    vec3 rayDir = ray.End - ray.Start;

    float rayLength = length(rayDir.xy);
    float near = ray.Start.z, far = ray.End.z;

    int mip = ray.BaseMip;
    int maxMip = textureQueryLevels(Camera.Depth) - 1;

#ifdef RAY_MAX_MIP
    maxMip = min(maxMip, RAY_MAX_MIP);
#endif

    RayResult result = RayResult(ray.Start, 0.0, vec3(0.0), RAY_RESULT_NO_HIT);

    int i;
    for(i = 0; i < settings.Iterations; i++)
    {
        vec3 oldPos = result.Position;
        ivec2 mipSize = textureSize(Camera.Depth, mip);
        ivec2 mipSizeUp = textureSize(Camera.Depth, mip + RAY_MIP_SKIP);
        ivec2 oldCell = SS_UVToTexel(result.Position.xy, mipSizeUp);

        result.Position = SS_CrossCell(result.Position, rayDir, mipSize, 1.0);

        ivec2 newCell = SS_UVToTexel(result.Position.xy, mipSizeUp);

        vec3 uv = result.Position;
        float lerp = distance(ray.Start.xy, uv.xy) / rayLength;
        uv.z = SS_CorrectDepth(near, far, lerp);

        if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
        {
            result.Result = RAY_RESULT_OUT_OF_BOUNDS;
            break;
        }

        float depth = textureLod(Camera.Depth, uv.xy, float(mip)).r;
        depth = SS_GetDepthWithBias(depth, SS_BIAS);

        if(depth < uv.z)
            if(mip == ray.BaseMip)
            {
                bool crossed = uv.z < depth + thickness;
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

    if(i == settings.Iterations) result.Result = RAY_RESULT_EXHAUSTED;
    return result;
}

RayResult SS_TraceRough(SSRay ray, SSLinearRaySettings settings)
{
    float thickness = settings.Base.Thickness > 0 ? settings.Base.Thickness : FLT_INF;

    float searchBias = CreateBias(settings.SearchBias);

    float near = ray.Start.z, far = ray.End.z;

    RayResult result = RayResult(ray.Start, 0.0, vec3(0.0), RAY_RESULT_NO_HIT);
    if(far < Camera.Planes.x) return result;

    int i;
    for(i = 0; i < settings.Base.Iterations; i++)
    {
        float lerp = pow(float(i) / (settings.Base.Iterations - 1), searchBias);

        vec3 uv = result.Position = mix(ray.Start, ray.End, lerp);
        uv.z = SS_CorrectDepth(near, far, lerp);

        if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) break;

        float depth = textureLod(Camera.Depth, uv.xy, float(ray.BaseMip)).r;
        depth = SS_GetDepthWithBias(depth, SS_BIAS);

        float offset = uv.z - depth;
        if(offset > 0.0)
        {
            result.Result = offset < thickness ? RAY_RESULT_HIT : RAY_RESULT_TOO_FAR;
            return result;
        }
    }

    if(i == settings.Base.Iterations) result.Result = RAY_RESULT_EXHAUSTED;
    return result;
}

float SS_ComputeAO(AOSettingsInternal settings, Vertex vert)
{
    float ao = 0.0;
    for(int s = 0; s < settings._base.Iterations; s++)
    {
        vec2 disk = VogelDisk(s, settings._base.Iterations, IGNSample * PI * 2.0);
        vec3 samplePos = vert.Position + vert.TBN * (normalize(vec3(disk, IGNSample)) * settings._base.Radius);
        vec3 uv = SS_WorldToUV(samplePos);

        float delta = uv.z - SS_GetDepthWithBias(textureLod(Camera.Depth, uv.xy, 0.0).x, SS_BIAS);
        if(delta > 0 && delta < settings._base.Thickness) ao += 1.0;
    }
    return SS_GetAOStrength(1.0 - ao / settings._base.Iterations, settings._base.Intensity);
}

float SS_AO(AOSettings s, Vertex vert)
{
    //vert.Position = SS_WorldToUV(vert.Position);
    //vert.Normal = SS_DirToView(vert.Normal);
    float depth = vert.Position.z;

    AOSettingsInternal settings;
    settings._base = s;
    settings.RadiusPixels = s.Radius * 0.5 / tan(Camera.Parameters.x * 0.5) * Camera.Size.y;
    settings.InvRadiusS = -1.0 / (settings.RadiusPixels * settings.RadiusPixels);
    settings.RadiusPixels /= depth;

    //if(settings.RadiusPixels < 1.0) return 1.0;
    return SS_ComputeAO(settings, vert);
}

// TODO: CLIP RAY TO SCREEN BOUNDS
void ClipSSRay(inout SSRay ray)
{
    if(ray.End.z > Camera.Planes.x) return;

    float nearPlane = Camera.Planes.x;
    float near = ray.End.z;
    float far = ray.Start.z;
    vec3 rayDir = ray.End - ray.Start;

    float rayTime = near - far;
    float collisionTime = (1.0 - EPSILON) * (rayTime - near) / rayTime;

    ray.End = ray.Start + rayDir * collisionTime;
}

SSRay CreateSSRayHiZ(Ray ray, SSRaySettings settings)
{
    SSRay result;

    ray.Direction = normalize(ray.Direction);
    ray.Position += settings.Normal * settings.NormalBias * (ray.BaseMip + 1);

    result.Start = SS_WorldToView(ray.Position);
    result.End = SS_WorldToView(ray.Position + ray.Direction * ray.Length);

    ClipSSRay(result);

    result.Start = SS_ViewToUV(result.Start);
    result.End = SS_ViewToUV(result.End);

    return result;
}

SSRay CreateSSRayLinear(Ray ray, SSLinearRaySettings settings)
{
    SSRay result;

    ray.Direction = normalize(ray.Direction);
    ray.Position += settings.Base.Normal * settings.Base.NormalBias * (ray.BaseMip + 1);
    ray.Position += ray.Direction * IGNSample * settings.RayBias / settings.Base.Iterations * (ray.BaseMip + 1);

    result.Start = SS_WorldToView(ray.Position);
    result.End = SS_WorldToView(ray.Position + ray.Direction * ray.Length);

    ClipSSRay(result);

    result.Start = SS_ViewToUV(result.Start);
    result.End = SS_ViewToUV(result.End);
    result.BaseMip = ray.BaseMip;

    return result;
}
#endif


float SS_GetAOStrength(float ao, float strength)
{
    if(strength == 0)
    return 1.0;
    return pow(ao, strength);
}