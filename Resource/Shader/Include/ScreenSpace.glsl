#include "Camera.glsl"
#include "Voxel.glsl"
#include "Noise.glsl"

#ifndef RAY_MAX_ITERATIONS
    #define RAY_MAX_ITERATIONS 64
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
vec2 SS_AlignUVToCell(vec2, ivec2);
float SS_CrossCell(inout vec3, vec3, ivec2);
vec3 SS_DirToView(vec3);
float LengthSquared(vec3 v) { return dot(v, v); }
float SS_GetDepthWithBias(float d, float bias) { return d + min(RAY_MAX_BIAS, d * bias); }
float SS_Sign(float f) { return f < 0 ? -1 : 1; }
vec2 SS_Sign(vec2 v) { return vec2(SS_Sign(v.x), SS_Sign(v.y)); }

// Implementation
#if defined(EXT_BINDLESS) && defined(FRAGMENT_SHADER)
float SS_CrossCell(inout vec3 pos, vec3 dir, ivec2 size)
{
    vec2 cellSize = 0.5 / size;

    vec2 planes = SS_AlignUVToCell(pos.xy, size) + cellSize;
    planes += sign(dir.xy) * cellSize;

    vec2 solution = (planes - pos.xy) / dir.xy;
    float dist = min(solution.x, solution.y);

    float cross = RAY_CELL_EPSILON * dist;

    pos += (dist - cross) * dir;

    return cross;
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

RayResult SS_Trace(Ray ray, out int m)
{
    ray.Direction = normalize(ray.Direction);

    vec3 rayStart = SS_WorldToUV(ray.Position);
    vec3 rayEnd = SS_WorldToUV(ray.Position + ray.Direction * ray.Length);
    vec3 rayDir = rayEnd - rayStart;

    float rayLength = length(rayDir.xy);
    float near = rayStart.z, far = rayEnd.z;
    float cross, oldCross;

    int mip = 0, mipCount = textureQueryLevels(Camera.Depth);

#ifdef RAY_MAX_MIP
    int maxMip = min(mipCount - 1, RAY_MAX_MIP);
#endif

    RayResult result = RayResult(rayStart, 0.0, vec3(0.0), RAY_RESULT_NO_HIT);

    rayDir /= abs(rayDir.z);
    if(rayDir.z < RAY_EPSILON) return result; // Temporary while I debug backwards rays.

    int i;
    for(i = 0; i < RAY_MAX_ITERATIONS; i++)
    {
        m = mip;
        vec3 oldPos = result.Position;
        ivec2 cell = SS_UVToTexel(result.Position.xy, textureSize(Camera.Depth, mip + 1));

        oldCross = cross;
        cross = SS_CrossCell(result.Position, rayDir, textureSize(Camera.Depth, mip));

        float lerp = distance(rayStart.xy, result.Position.xy) / rayLength;

        vec3 uv = result.Position;
        uv.z = (near * far) / (near * lerp + far * (1.0 - lerp));

        if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)
        {
            result.Result = RAY_RESULT_OUT_OF_BOUNDS;
            break;
        }

        float depth = textureLod(Camera.Depth, uv.xy, float(mip)).r;
        depth = SS_GetDepthWithBias(depth, 0.01);

        if(depth < uv.z)
            if(mip == 0)
            {
                float lerp = distance(rayStart.xy, oldPos.xy) / rayLength;
                float prevZ = (near * far) / (near * lerp + far * (1.0 - lerp));

                bool crossed = uv.z < depth + RAY_THICKNESS;
                //crossed = crossed || (!crossed && prevZ < depth);

                result.Result = crossed ? RAY_RESULT_HIT : RAY_RESULT_TOO_FAR;
                break;
            }
            else
            {
                mip--;
                result.Position = oldPos;
                // SS_CrossCell(result.Position, -rayDir, textureSize(Camera.Depth, mip));
            }
        else
        {
            result.Position += rayDir * cross * 2;
            ivec2 newCell = SS_UVToTexel(result.Position.xy, textureSize(Camera.Depth, mip + 1));

            if(cell != newCell) mip++;

        #ifndef RAY_MAX_MIP
            if(mip == mipCount) break;
        #else
            mip = min(mip, maxMip);
        #endif
        }
    }

    if(i == RAY_MAX_ITERATIONS) result.Result = RAY_RESULT_EXHAUSTED;
    return result;
}

RayResult SS_TraceRough(Ray ray, LinearRaySettings settings)
{
    ray.Direction = normalize(ray.Direction);

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
        result.Position += rayDir;

        float lerp = float(i + 1) / settings.Iterations;
        vec3 uv = result.Position;
        uv.z = (near * far) / (near * lerp + far * (1.0 - lerp));

        if(uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0 || uv.z < 0.0) break;

        float depth = textureLod(Camera.Depth, uv.xy, 0.f).r;

        if(SS_GetDepthWithBias(depth, 0.05) < uv.z)
        {
            result.Result = uv.z - depth < RAY_THICKNESS ? RAY_RESULT_HIT : RAY_RESULT_TOO_FAR;
            break;
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