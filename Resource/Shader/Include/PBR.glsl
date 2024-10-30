#include "Scene.glsl"
#include "Camera.glsl"
#include "Shadow.glsl"
#include "Vertex.glsl"
#include "Voxel.glsl"
#include "ScreenSpace.glsl"
#include "Math.glsl"

#ifndef PI
    #define PI 3.141592
#endif

#ifndef EPSILON
    #define EPSILON 0.001
#endif

#define HAMMERSLEY_ROUGHNESS_SAMPLE (TAA_SAMPLE_SQUARED * TAA_SAMPLE_SQUARED)

#ifdef FRAGMENT_SHADER
    uniform sampler2D BRDFLutTex;
#endif

#ifndef PBR_VOXEL_LERP_BEGIN
    #define PBR_VOXEL_LERP_BEGIN 0.8
#endif

struct PBRFragment
{
    vec3 Normal;
    float Roughness;
    vec3 Albedo;
    float Metallic;
    vec3 F0;
    float Specular;
};

struct PBRSample
{
    vec2 BRDF;
    vec3 F;
    vec3 Specular;
    vec3 Diffuse;
};

// Main Functions
#ifdef FRAGMENT_SHADER
vec3 GetLighting(const Vertex, const PBRFragment, const Light, const vec4);
vec3 GetLightingDirectional(const Vertex, const PBRFragment, const Light, const vec4);
vec3 GetLightingPoint(const Vertex, const PBRFragment, const Light);
vec3 GetLightingSpecular(const PBRSample, vec3);
#endif

// PBR Functions
// https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float GGXNDF(float nDotV, float roughness);
float GGXNDFPoint(float nDotV, float roughness, float radius, float distance);
float GGXNDFAlpha(float nDotV, float alpha);
float GSchlick(float cosTheta, float roughness); // AKA 'k'
float GSchlick(float nDotL, float nDotH, float roughness);
float GSchlickAnalytical(float nDotL, float nDotH, float roughness); // Read more: Section 3, "Specular G"
vec3 FresnelSchlick(vec3 f0, float nDotV);
float FalloffPoint(float distance, float maxDistance);
vec3 CubemapParallax(vec3 pos, vec3 dir, Cubemap cubemap, out float weight);
#ifdef FRAGMENT_SHADER
vec3 FilterSpecular(const Vertex vert, const PBRFragment frag, const PBRSample pbrSample, vec3 color);
#endif
vec3 ImportanceSampleGGX(vec2 xi, vec3 n, float roughness);
float VDCInverse(uint bits);
vec2 Hammersley(uint i, uint sampleCount);

// Implementation
// Main Functions
#ifdef FRAGMENT_SHADER
vec3 GetLighting(const Vertex vert, const PBRFragment frag, const Light light, const vec4 fragLightSpace)
{
    switch(light.Type)
    {
        case LIGHT_DIRECTIONAL: return GetLightingDirectional(vert, frag, light, fragLightSpace);
        case LIGHT_POINT: return GetLightingPoint(vert, frag, light);
        default: return vec3(0.0);
    }
}

vec3 GetLightingDirectional(const Vertex vert, const PBRFragment frag, const Light light, const vec4 fragLightSpace)
{
    // Vector Setup
    vec3 eye = normalize(Camera.Position - vert.Position);
    vec3 halfEye = normalize(light.Position + eye);

    float nDotV = saturate(dot(frag.Normal, eye));
    float nDotL = saturate(dot(frag.Normal, light.Position));
    float nDotH = saturate(dot(frag.Normal, halfEye));
    float eDotH = saturate(dot(halfEye, eye));

    // PBR Setup
    vec3 f0 = mix(frag.F0, frag.Albedo, frag.Metallic);
    vec3 f = FresnelSchlick(f0, eDotH);
    float d = GGXNDF(nDotH, frag.Roughness);
    float g = GSchlickAnalytical(nDotL, nDotV, frag.Roughness);

    vec3 kD = (1.0 - f) * (1.0 - frag.Metallic);

    // Final Calculations
    vec3 specularBRDF = (f * d * g) / max(4.0 * nDotL * nDotV, EPSILON);
    vec3 diffuseBRDF = frag.Albedo * kD;

    // Falloff of nDotL * nDotL is nicer to me
    float lambert = min(nDotL, GetShadowDirectional(vert, light, fragLightSpace));

    return (diffuseBRDF + specularBRDF) * lambert * light.Color;
}

vec3 GetLightingPoint(const Vertex vert, const PBRFragment frag, const Light light)
{
    float radius = uintBitsToFloat(light.PackedSettings);

    // Vector Setup
    vec3 eye = normalize(Camera.Position - vert.Position);
    vec3 lightDir = light.Position - vert.Position;

    // Closest point to frag
    vec3 reflection = reflect(eye, frag.Normal);
    vec3 centerToRay = dot(lightDir, reflection) * reflection - lightDir;
    vec3 closestPoint = lightDir + centerToRay * saturate(radius / length(centerToRay));

    float closestDistance = length(closestPoint);
    vec3 closestDir = closestPoint / closestDistance;

    float lightDistance = length(lightDir);
    lightDir /= lightDistance;
    lightDistance = max(lightDistance - radius, 0.0);

    // Vector Setup
    vec3 halfEye = normalize(closestDir + eye);

    float nDotV = saturate(dot(frag.Normal, eye));
    float nDotL = saturate(dot(frag.Normal, closestDir));
    float nDotH = saturate(dot(frag.Normal, halfEye));
    float eDotH = saturate(dot(halfEye, eye));

    // PBR Setup
    vec3 f0 = mix(frag.F0, frag.Albedo, frag.Metallic);
    vec3 f = FresnelSchlick(f0, eDotH);
    float d = GGXNDFPoint(nDotH, frag.Roughness, radius, lightDistance);
    float g = GSchlickAnalytical(nDotL, nDotV, frag.Roughness);

    vec3 kD = (1.0 - f) * (1.0 - frag.Metallic);

    // Final Calculations
    vec3 specularBRDF = vec3(f * d * g) / max(4.0 * nDotL * nDotV, EPSILON);
    vec3 diffuseBRDF = frag.Albedo * kD / PI;

    // Falloff of nDotL * nDotL is nicer to me
    float lambert = saturate(dot(frag.Normal, lightDir)) * 0.5 + 0.5;
    lambert = min(lambert, GetShadowPoint(vert, light));
    float attenuation = FalloffPoint(lightDistance, 10.f);

    return (diffuseBRDF + specularBRDF) * attenuation * lambert * light.Color;
}

vec3 FilterSpecular(const Vertex vert, const PBRFragment frag, const PBRSample pbrSample, vec3 color)
{
    // PBR Setup
    vec3 eye = normalize(Camera.Position - vert.Position);

    float nDotV = saturate(dot(frag.Normal, eye));
    vec3 f0 = mix(frag.F0, frag.Albedo, frag.Metallic);
    vec3 f = FresnelSchlick(f0, nDotV);
    vec2 brdf = pbrSample.BRDF;

    return (brdf.g + f * brdf.r) * color;
}
#endif

// PBR Functions
float GGXNDF(float nDotH, float roughness)
{
    float alpha = max(roughness * roughness, EPSILON);
    return GGXNDFAlpha(nDotH, alpha);
}

float GGXNDFPoint(float nDotV, float roughness, float radius, float distance)
{
    float alpha = max(roughness * roughness, 0.01);
    float alphaPrime = roughness + 1.0 / (radius + 2 * distance);
    // Sorta deviated from Epic Games' paper, but it looks good to me so oh well.
    return GGXNDFAlpha(nDotV, alpha) * alphaPrime;
}

float GGXNDFAlpha(float nDotH, float alpha)
{
    float alphaSqr = max(alpha * alpha, 0.01);

    nDotH = mix(0.1, 1.0, nDotH);

    float denom = (nDotH * nDotH) * (alphaSqr - 1.0) + 1.0;
    denom = PI * denom * denom;

    return alphaSqr / denom;
}

float GSchlick(float cosTheta, float roughness)
{
    float denom = cosTheta * (1.0 - roughness) + roughness;
    return cosTheta / max(denom, EPSILON);
}

float GSchlick(float nDotL, float nDotV, float roughness)
{
    roughness = max(roughness, EPSILON);
    float k = (roughness * roughness) / 2.0;
    return GSchlick(nDotL, k) * GSchlick(nDotV, k);
}

float GSchlickAnalytical(float nDotL, float nDotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return GSchlick(nDotL, k) * GSchlick(nDotV, k);
}

float FalloffPoint(float distance, float maxDistance)
{
    float factor = 1.0 - min(distance, maxDistance) / maxDistance;
    factor = factor * factor * (3.0 - 2.0 * factor);

    return factor / (1.0 + distance * distance);
}

vec3 FresnelSchlick(vec3 f0, float nDotV)
{
    return f0 + (1.0 - f0) * pow(2.0, (-5.55473 * nDotV - 6.98316) * nDotV);
}

// Epic Games black magic
vec3 ImportanceSampleGGX(vec2 xi, vec3 n, float roughness)
{
    roughness = max(roughness, EPSILON);
    float a = roughness * roughness;

    float phi = PI * 2.0 * xi.x;
    float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 h = vec3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);

    return GetTBN(n) * h;
}

// How someone came up with this, I don't know.
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float VDCInverse(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint sampleCount)
{
    return vec2(float(i) / float(sampleCount), VDCInverse(i));
}

// https://seblagarde.wordpress.com/2012/09/29/image-based-lighting-approaches-and-parallax-corrected-cubemap/
vec3 CubemapParallax(vec3 pos, vec3 dir, Cubemap cubemap, out float weight)
{
    switch(cubemap.Type)
    {
        case CUBEMAP_AABB:

            vec3 boxMin = cubemap.Position - cubemap.Scale;
            vec3 boxMax = cubemap.Position + cubemap.Scale;

            vec3 first = (boxMin - pos) / dir;
            vec3 second = (boxMax - pos) / dir;

            first = max(first, second);
            float dist = min(first.x, min(first.y, first.z));

            weight = 1.0 - float(any(greaterThan(abs(pos - cubemap.Position), cubemap.Scale)));

            if(weight == 0.0) return dir;
            return (pos + dir * dist) - cubemap.Position;

        default:
            return dir;
    }
}

#ifdef FRAGMENT_SHADER
PBRSample ImportanceSample(const Vertex vert, const PBRFragment frag)
{
    vec3 eye = normalize(Camera.Position - vert.Position);
    float nDotV = max(dot(frag.Normal, eye), 0.0);

    vec2 xi = Hammersley(int(IGNSample * HAMMERSLEY_ROUGHNESS_SAMPLE), HAMMERSLEY_ROUGHNESS_SAMPLE);
    vec3 n = ImportanceSampleGGX(xi, frag.Normal, frag.Roughness);
    vec3 r = -normalize(reflect(eye, n));

    vec3 f0 = mix(frag.F0, frag.Albedo, frag.Metallic);
    vec3 f = FresnelSchlick(f0, nDotV);
    vec2 brdf = textureLod(BRDFLutTex, vec2(nDotV, frag.Roughness), 0.0).rg;

    return PBRSample(brdf, f, r, vec3(0.0));
}
#endif