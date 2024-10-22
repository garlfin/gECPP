#include "Camera.glsl"
#include "Bindless.glsl"
#include "Vertex.glsl"
#include "TAA.glsl"
#include "Noise.glsl"
#include "ScreenSpace.glsl"
#include "Math.glsl"

// In percent
#ifndef SHADOW_SAMPLES
    #define SHADOW_SAMPLES 32
#endif

#ifndef SMRT_SAMPLES
    #define SMRT_SAMPLES 6
#endif

#ifndef SMRT_TRACE_STEPS
    #define SMRT_TRACE_STEPS 12
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
    #define SMRT_CONTACT_SAMPLES SMRT_SAMPLES
#endif

#ifndef SMRT_CONTACT_TRACE_STEPS
    #define SMRT_CONTACT_TRACE_STEPS 32
#endif

#ifndef SMRT_COLOR_BIAS
    #define SMRT_COLOR_BIAS 10.0
#endif

#ifndef DIRECTIONAL_SHADOW_RADIUS
    #define DIRECTIONAL_SHADOW_RADIUS 0.1
#endif

#ifndef DIRECTIONAL_SHADOW_BIAS
    #define DIRECTIONAL_SHADOW_BIAS 0.01
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
    float nDotL = max(dot(vert.Normal, light.Position), 0.0);
    float bias = mix(0.1, 1.0, nDotL) * DIRECTIONAL_SHADOW_BIAS;
	vec3 fragPos = DirectionalPerspectiveDivide(fragLightSpace, light.Planes);

    if(TexcoordOutOfBounds(fragPos.xy) || fragPos.z > light.Planes.y) return 1.0;

    mat3 offsetMatrix = GetTBN(light.Position);
    float shadow = 0.0;

    for(int s = 0; s < SMRT_SAMPLES; s++)
    {
        vec2 offset = VogelDisk(s, SMRT_SAMPLES, IGNSample * PI * 2.0);
        vec3 rayDir = offsetMatrix * vec3(offset * DIRECTIONAL_SHADOW_RADIUS, 1.0) * SMRT_TRACE_DISTANCE;

        if(dot(rayDir, vert.Normal) < 0.0) continue;

        vec3 rayStart = vert.Position + rayDir;
        rayStart = DirectionalPerspectiveDivide(light.ViewProjection * vec4(rayStart, 1.0), light.Planes);

        vec3 rayEnd = vert.Position + rayDir * 0.005;
        rayEnd = DirectionalPerspectiveDivide(light.ViewProjection * vec4(rayEnd, 1.0), light.Planes);

        for(int i = 0; i < SMRT_TRACE_STEPS; i++)
        {
            float lerp = float(i + 1) / SMRT_TRACE_STEPS;
            lerp = pow(lerp, 1.0 / SMRT_TRACE_BIAS);

            vec3 rayPos = mix(rayStart, rayEnd, lerp);

            float depth = texture(sampler2D(light.Depth), rayPos.xy).r;
            depth = depth * 2.0 - 1.0;
            depth = LinearizeDepthOrtho(depth, light.Planes);

            if(depth > rayPos.z) continue;

            shadow += 1.0;
            break;
        }
    }

    shadow = clamp(shadow / SMRT_SAMPLES, 0.0, 1.0);
    shadow = pow(1.0 - shadow, SMRT_COLOR_BIAS);

#if defined(ENABLE_SMRT) && defined(ENABLE_SMRT_CONTACT_SHADOW)
    if(shadow < EPSILON) return shadow;

    Ray ray;
    LinearRaySettings raySettings = LinearRaySettings(SMRT_CONTACT_TRACE_STEPS, EPSILON, EPSILON, vert.Normal, SMRT_CONTACT_TRACE_BIAS);
    RayResult result;

    float contactShadow = 0.0;
    for(uint i = 0; i < SMRT_CONTACT_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i, SMRT_CONTACT_SAMPLES, IGNSample * PI * 2.0) * DIRECTIONAL_SHADOW_RADIUS;
        vec3 rayDir = offsetMatrix * vec3(offset, 1.0);

        ray = Ray(vert.Position, SMRT_TRACE_DISTANCE / 50.0, rayDir);
        result = SS_TraceRough(ray, raySettings);

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

    float nDotL = max(dot(vert.Normal, lightDir), 0.0);
    float bias = (1.0 + nDotL) * POINT_SHADOW_BIAS * lightRadius * 10.0;

    float baseSampleDepth = max(abs(baseSamplePos.x), max(abs(baseSamplePos.y), abs(baseSamplePos.z)));

    if(baseSampleDepth < light.Planes.x || baseSampleDepth > light.Planes.y) return 1.0;

    mat3 offsetMatrix = GetTBN(lightDir);

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

#if defined(DIRECTIONAL_CONTACT_SHADOW) && CONTACT_SAMPLES > 0
    if(shadow < EPSILON) return shadow;

    Ray ray;
    LinearRaySettings raySettings = LinearRaySettings(32, bias, 0.001, vert.Normal, 1.5);
    RayResult result;

    float contactShadow = 0.0;

    for(uint i = 0; i < CONTACT_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i, CONTACT_SAMPLES, IGNSample * PI * 2.0) * lightRadius;
        vec3 lightOffset = offsetMatrix * vec3(offset, 1.0);

        vec3 rayDir = (light.Position + lightOffset) - vert.Position;

        ray = Ray(vert.Position, POINT_CONTACT_SHADOW_LENGTH, rayDir);
        result = SS_TraceRough(ray, raySettings);

        contactShadow += float(result.Result != RAY_RESULT_HIT);
    }

    shadow = min(shadow, contactShadow / CONTACT_SAMPLES);

    if(result.Result == RAY_RESULT_HIT) shadow = 0.0;
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
	frag.z = LinearizeDepthOrtho(frag.z, planes);
	return frag.xyz;
}