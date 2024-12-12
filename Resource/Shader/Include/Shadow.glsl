#include "Camera.glsl"
#include "Bindless.glsl"
#include "Vertex.glsl"
#include "TAA.glsl"
#include "Noise.glsl"
#include "ScreenSpace.glsl"
#include "Math.glsl"

#ifndef SHADOW_SAMPLES
    #define SHADOW_SAMPLES 16
#endif

#ifndef SMRT_SAMPLES
    #define SMRT_SAMPLES 6
#endif

#ifndef SMRT_TRACE_STEPS
    #define SMRT_TRACE_STEPS 16
#endif

#ifndef SMRT_DISTANCE
    #define SMRT_TRACE_DISTANCE 10.0
#endif

#ifndef SMRT_TRACE_BIAS
    #define SMRT_TRACE_BIAS 10.0
#endif

#ifndef SMRT_CONTACT_TRACE_BIAS
    #define SMRT_CONTACT_TRACE_BIAS 1.5
#endif

#ifndef SMRT_CONTACT_SAMPLES
    #define SMRT_CONTACT_SAMPLES 8
#endif

#ifndef SMRT_CONTACT_TRACE_STEPS
    #define SMRT_CONTACT_TRACE_STEPS 16
#endif

#ifndef SMRT_CONTACT_LENGTH
    #define SMRT_CONTACT_TRACE_LENGTH 0.1
#endif

#ifndef SMRT_COLOR_BIAS
    #define SMRT_COLOR_BIAS 10.0
#endif

#ifndef DIRECTIONAL_SHADOW_RADIUS
    #define DIRECTIONAL_SHADOW_RADIUS 0.1
#endif

#ifndef DIRECTIONAL_SHADOW_BIAS
    #define DIRECTIONAL_SHADOW_BIAS DIRECTIONAL_SHADOW_RADIUS
#endif

#ifndef POINT_SHADOW_BIAS
    #define POINT_SHADOW_BIAS 0.001
#endif

// Main Functions
#ifdef FRAGMENT_SHADER
    float GetShadowDirectional(const Vertex frag, const Light light, const vec4 fragLightSpace);
    float GetShadowPoint(const Vertex frag, const Light light);
#endif

// Helper Functions
vec3 DirectionalPerspectiveDivide(vec4, vec2);

// Implementation
// Main Functions
#ifdef FRAGMENT_SHADER
float GetShadowDirectional(const Vertex vert, const Light light, const vec4 fragLightSpace)
{
#ifdef EXT_BINDLESS
    float viewDistance = distance(vert.Position, Camera.Position);
    float nDotL = max(dot(vert.Normal, light.Position), 0.0);
    float bias = 1.0 - nDotL + EPSILON;

	vec3 fragPos = DirectionalPerspectiveDivide(fragLightSpace, light.Planes);

    bool calculateShadow = !(TexcoordOutOfBounds(fragPos.xy) || fragPos.z > light.Planes.y);

#if defined(ENABLE_SMRT) || defined(ENABLE_SMRT_CONTACT_SHADOW)
    mat3 offsetMatrix = GetTBN(light.Position);
#endif

     float shadow = calculateShadow ? 0.0 : 1.0;

#ifndef ENABLE_SMRT
    if(calculateShadow)
    {   for(int s = 0; s < SHADOW_SAMPLES; s++)
        {
            vec2 offset = VogelDisk(s, SHADOW_SAMPLES, IGNSample * PI * 2.0);
            vec3 uv = fragPos.xyz;
            uv.xy += offset * DIRECTIONAL_SHADOW_RADIUS * 0.5 / light.PackedSettings;

            float depth = texture(sampler2D(light.Depth), uv.xy).r;
            depth = LinearizeDepthOrtho(depth, light.Planes);

            if(uv.z < depth + bias * DIRECTIONAL_SHADOW_BIAS) shadow += 1.0;
        }

        shadow /= SHADOW_SAMPLES;
    }
#else
    if(calculateShadow)
    {
        for(int s = 0; s < SMRT_SAMPLES; s++)
        {
            vec2 offset = VogelDisk(s, SMRT_SAMPLES, IGNSample * PI * 2.0);
            vec3 rayDir = offsetMatrix * vec3(offset * DIRECTIONAL_SHADOW_RADIUS, 1.0) * SMRT_TRACE_DISTANCE;

            vec3 rayStart = vert.Position + rayDir;
            rayStart = DirectionalPerspectiveDivide(light.ViewProjection * vec4(rayStart, 1.0), light.Planes);

            vec3 rayEnd = vert.Position + rayDir * 0.002 * bias;
            rayEnd = DirectionalPerspectiveDivide(light.ViewProjection * vec4(rayEnd, 1.0), light.Planes);

            for(int i = 0; i < SMRT_TRACE_STEPS; i++)
            {
                float lerp = pow(float(i) / (SMRT_TRACE_STEPS - 1), 1.0 / SMRT_TRACE_BIAS);

                vec3 rayPos = mix(rayStart, rayEnd, lerp);

                float depth = texture(sampler2D(light.Depth), rayPos.xy).r;
                depth = LinearizeDepthOrtho(depth, light.Planes);

                if(depth > rayPos.z) continue;

                shadow += 1.0;
                break;
            }
        }

        shadow = saturate(shadow / SMRT_SAMPLES);
        shadow = pow(1.0 - shadow, SMRT_COLOR_BIAS);
    }
#endif
#ifdef ENABLE_SMRT_CONTACT_SHADOW
    if(shadow < EPSILON) return shadow;

    SSLinearRaySettings raySettings = SSLinearRaySettings
    (
        SMRT_CONTACT_TRACE_STEPS,
        0.01,
        1.0,
        0.2,
        vert.Normal,
        SMRT_CONTACT_TRACE_BIAS
    );

    float contactShadow = 0.0;
    for(uint i = 0; i < SMRT_CONTACT_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i, SMRT_CONTACT_SAMPLES, IGNSample * PI * 2.0) * DIRECTIONAL_SHADOW_RADIUS;
        vec3 rayDir = offsetMatrix * vec3(offset, 1.0);

        Ray ray = Ray(vert.Position, viewDistance * SMRT_CONTACT_TRACE_LENGTH, rayDir);
        SSRay ssRay = CreateSSRayLinear(ray, raySettings);
        RayResult result = SS_TraceRough(ssRay, raySettings);

        contactShadow += float(result.Result != RAY_RESULT_HIT);
    }

    shadow = min(shadow, contactShadow / SMRT_CONTACT_SAMPLES);
#endif
    return shadow;
#else
    return 1.0;
#endif
}

float GetShadowPoint(const Vertex vert, const Light light)
{
#ifdef EXT_BINDLESS
    vec3 baseSamplePos = vert.Position - light.Position;
    vec3 lightDir = -normalize(baseSamplePos);
    float lightRadius = uintBitsToFloat(light.PackedSettings);

    float viewDistance = distance(vert.Position, Camera.Position);
    float nDotL = max(dot(vert.Normal, lightDir), 0.0);
    float bias = (1.0 + nDotL) * POINT_SHADOW_BIAS * lightRadius * 10.0;

    float baseSampleDepth = max(abs(baseSamplePos.x), max(abs(baseSamplePos.y), abs(baseSamplePos.z)));

    if(baseSampleDepth < light.Planes.x || baseSampleDepth > light.Planes.y) return 1.0;

    float blocker = lightRadius * 0.1;
    float shadow = 0.0;

/*
#ifdef SOFT_SHADOW_AVERAGE
    float blockerCount = 0.0;
#endif
#if SOFT_SHADOW
    for(uint i = 0; i < SHADOW_BLOCKER_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i, SHADOW_SAMPLES, IGNSample * PI * 2.0) * lightRadius;
        vec3 samplePos = baseSamplePos + offsetMatrix * vec3(offset, 0.0);

        float sampleDepth = max(abs(samplePos.x), max(abs(samplePos.y), abs(samplePos.z)));
        float logSampleDepth = LogarithmizeDepthNDC(sampleDepth, light.Planes);

        float depth = texture(samplerCube(light.Depth), samplePos).r;
        float linearDepth = LinearizeDepthNDC(depth, light.Planes);

        float shadowSample = depth + bias < logSampleDepth ? 1.0 : 0.0;

    #ifdef SOFT_SHADOW_AVERAGE
        blocker += linearDepth * shadowSample;
        blockerCount += shadowSample;
    #else
        blocker = max(blocker, depth * shadowSample);
    #endif
    }

    #ifdef SOFT_SHADOW_AVERAGE
        blockerCount = max(blockerCount, 1.f);
        blocker /= blockerCount;
    #endif
    blocker = (baseSampleDepth - blocker) / lightRadius * blocker;
    blocker = clamp(blocker, POINT_SHADOW_MIN_RADIUS, lightRadius);
#else
    blocker = lightRadius * 0.1;
#endif*/

    mat3 offsetMatrix = GetTBN(light.Position);

    for(int i = 0; i < SHADOW_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i, SHADOW_SAMPLES, IGNSample * PI * 2.0) * blocker;
        vec3 samplePos = baseSamplePos + offsetMatrix * vec3(offset, 0.0);

        float depth = texture(samplerCube(light.Depth), samplePos).r;
        float sampleDepth = max(abs(samplePos.x), max(abs(samplePos.y), abs(samplePos.z)));
        sampleDepth = LogarithmizeDepthNDC(sampleDepth, light.Planes);

        shadow += depth + bias < sampleDepth ? 0.0 : 1.0;
    }

    shadow /= SHADOW_SAMPLES;

#if false
    if(shadow < EPSILON) return shadow;

    SSLinearRaySettings raySettings = SSLinearRaySettings
    (
        SMRT_CONTACT_TRACE_STEPS,
        EPSILON,
        EPSILON,
        0.2,
        vert.Normal,
        SMRT_CONTACT_TRACE_BIAS
    );

    float contactShadow = 0.0;
    for(uint i = 0; i < SMRT_CONTACT_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i + 1, SMRT_CONTACT_SAMPLES + 1, IGNSample * PI * 2.0) * lightRadius;
        vec3 lightOffset = offsetMatrix * vec3(offset, 1.0);

        vec3 rayDir = (light.Position + lightOffset) - vert.Position;

        Ray ray = Ray(vert.Position, viewDistance * SMRT_CONTACT_TRACE_LENGTH, rayDir);
        SSRay ssRay = CreateSSRayLinear(ray, raySettings);
        RayResult result = SS_TraceRough(ssRay, raySettings);

        contactShadow += float(!(result.Result == RAY_RESULT_HIT));
    }

    shadow = min(shadow, contactShadow / SMRT_CONTACT_SAMPLES);
#endif
    return shadow;
#else
    return 1.0;
#endif
}
#endif

// Helper Functions
vec3 DirectionalPerspectiveDivide(vec4 frag, vec2 planes)
{
	frag.xyz = frag.xyz / frag.w;
	frag.xy = frag.xy * 0.5 + 0.5;
	frag.z = LinearizeDepthOrthoNDC(frag.z, planes);
	return frag.xyz;
}