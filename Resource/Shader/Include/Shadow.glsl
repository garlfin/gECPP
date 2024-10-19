#include "Camera.glsl"
#include "Bindless.glsl"
#include "Vertex.glsl"
#include "TAA.glsl"
#include "Noise.glsl"
#include "ScreenSpace.glsl"
#include "Math.glsl"

// In percent
#ifndef DIRECTIONAL_SHADOW_BIAS
    #define DIRECTIONAL_SHADOW_BIAS 0.01
#endif

#ifndef POINT_SHADOW_BIAS
    #define POINT_SHADOW_BIAS 0.001
#endif

#ifndef SHADOW_SAMPLES
    #define SHADOW_SAMPLES 32
#endif

#ifndef CONTACT_SAMPLES
    #define CONTACT_SAMPLES 4
#endif

#define SOFT_SHADOW (defined(SOFT_SHADOW_AVERAGE) || defined(SOFT_SHADOW_MIN))

#if SOFT_SHADOW
    #ifndef SHADOW_BLOCKER_SAMPLES
        #define SHADOW_BLOCKER_SAMPLES 8
    #endif

    #ifndef DIRECTIONAL_SHADOW_BLOCKER_RADIUS
        #define DIRECTIONAL_SHADOW_BLOCKER_RADIUS 0.1
    #endif

    #ifndef POINT_SHADOW_BLOCKER_RADIUS
        #define POINT_SHADOW_BLOCKER_RADIUS 0.1
    #endif

    #ifndef DIRECTIONAL_SHADOW_MIN_RADIUS
        #define DIRECTIONAL_SHADOW_MIN_RADIUS 0.01
    #endif

    #ifndef POINT_SHADOW_MIN_RADIUS
        #define POINT_SHADOW_MIN_RADIUS 0.01
    #endif

    #ifndef POINT_CONTACT_SHADOW_LENGTH
        #define POINT_CONTACT_SHADOW_LENGTH 0.1
    #endif
#endif

#ifndef DIRECTIONAL_SHADOW_RADIUS
    #define DIRECTIONAL_SHADOW_RADIUS 0.1
#endif

// Main Functions
#ifdef FRAGMENT_SHADER
    float GetShadowDirectional(const Vertex frag, const Light light, const vec4 fragLightSpace);
    float GetShadowPoint(const Vertex frag, const Light light);
#endif

// Helper Functions
vec3 NDCLight(vec4, vec2);

// Implementation
// Main Functions
#ifdef FRAGMENT_SHADER
float GetShadowDirectional(const Vertex vert, const Light light, const vec4 fragLightSpace)
{
#ifdef EXT_BINDLESS
    float nDotL = max(dot(vert.Normal, light.Position), 0.0);
    float bias = mix(0.1, 1.0, nDotL) * DIRECTIONAL_SHADOW_BIAS;
	vec3 fragPos = NDCLight(fragLightSpace, light.Planes);

    float blocker = 0.0;
#ifdef SOFT_SHADOW_AVERAGE
    float blockerCount = 0.0;
#endif
    float shadow = 0.0;

#if SOFT_SHADOW
    for(uint i = 0; i < SHADOW_BLOCKER_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i, SHADOW_BLOCKER_SAMPLES, IGNSample * PI * 2.0);
        vec3 uv = fragPos;
        uv.xy += offset * DIRECTIONAL_SHADOW_BLOCKER_RADIUS / light.PackedSettings;

        float z = texture(sampler2D(light.Depth), uv.xy).r;
        z = LinearizeDepthOrtho(z, light.Planes);
        z = uv.z - z;

        float shadowSample = z > bias ? 1.0 : 0.0;

        #ifdef SOFT_SHADOW_AVERAGE
            blocker += z * shadowSample;
            blockerCount += shadowSample;
        #else
            blocker = max(blocker, z * shadowSample);
        #endif
    }

    #ifdef SOFT_SHADOW_AVERAGE
        blockerCount = max(blockerCount, 1.f);
        blocker /= blockerCount;
    #endif
    blocker = max(blocker * DIRECTIONAL_SHADOW_RADIUS, DIRECTIONAL_SHADOW_MIN_RADIUS);
#else
    blocker = DIRECTIONAL_SHADOW_RADIUS;
#endif

    for(uint i = 0; i < SHADOW_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i, SHADOW_SAMPLES, IGNSample * PI * 2.0);
        vec3 uv = fragPos;
        uv.xy += offset * blocker / light.PackedSettings;

    #ifdef EXT_BINDLESS
        float z = texture(sampler2D(light.Depth), uv.xy).r;
    #else
        float z = 1.0;
    #endif

        z = LinearizeDepthOrtho(z, light.Planes);
        z = uv.z - z;

        float shadowSample = z > bias ? 0.0 : 1.0;

        shadowSample = TexcoordOutOfBounds(uv.xy) ? 1.0 : shadowSample;
        shadowSample = uv.z > light.Planes.y ? 1.0 : shadowSample;

        shadow += shadowSample;
    }

    shadow /= SHADOW_SAMPLES;

#if defined(DIRECTIONAL_CONTACT_SHADOW) && CONTACT_SAMPLES > 0
    if(shadow < EPSILON) return shadow;

    Ray ray;
    LinearRaySettings raySettings = LinearRaySettings(32, EPSILON, EPSILON, vert.Normal);
    RayResult result;

    float contactShadow = 0.0;
    mat3 offsetMatrix = GetTBN(light.Position);
    for(uint i = 0; i < CONTACT_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i, CONTACT_SAMPLES, IGNSample * PI * 2.0) * DIRECTIONAL_SHADOW_RADIUS;
        vec3 rayDir = offsetMatrix * vec3(offset, 1.0);

        ray = Ray(vert.Position, DIRECTIONAL_SHADOW_MIN_RADIUS / DIRECTIONAL_SHADOW_RADIUS, rayDir);
        result = SS_TraceRough(ray, raySettings);

        contactShadow += float(result.Result != RAY_RESULT_HIT);
    }

    shadow = min(shadow, contactShadow / CONTACT_SAMPLES);
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
    LinearRaySettings raySettings = LinearRaySettings(32, bias, 0.001, vert.Normal);
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
vec3 NDCLight(vec4 frag, vec2 planes)
{
	frag.xyz = frag.xyz / frag.w;
	frag.xyz = frag.xyz * 0.5 + 0.5;
	frag.z = LinearizeDepthOrtho(frag.z, planes);
	return frag.xyz;
}