#include "Scene.glsl"
#include "Camera.glsl"
#include "Shadow.glsl"
#include "Vertex.glsl"
#include "Voxel.glsl"

#ifndef PI
    #define PI 3.141592
#endif

#ifndef EPSILON
    #define EPSILON 0.001
#endif

#define HAMMERSLEY_ROUGHNESS_SAMPLE TAA_SAMPLE_SQUARED

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
vec3 GetLighting(const Vertex, const PBRFragment, const Light);
vec3 GetLightingDirectional(const Vertex, const PBRFragment, const Light);

vec3 GetLighting(const Vertex, const PBRFragment, const PBRSample, samplerCube);
#ifdef EXT_BINDLESS
    vec3 GetLighting(const Vertex, const PBRFragment, const PBRSample, const Cubemap);
    vec4 GetLighting(const Vertex, const PBRFragment, const PBRSample, const VoxelGridData);
#endif
#endif

// PBR Functions
// https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float GGXNDF(float nDotV, float roughness);
float GSchlick(float cosTheta, float roughness); // AKA 'k'
float GSchlick(float nDotL, float nDotH, float roughness);
float GSchlickAnalytical(float nDotL, float nDotH, float roughness); // Read more: Section 3, "Specular G"
vec3 FresnelSchlick(vec3 f0, float nDotV);
vec3 CubemapParallax(vec3, vec3, Cubemap);
#ifdef FRAGMENT_SHADER
PBRSample ImportanceSample(const Vertex, const PBRFragment);
#endif
vec3 ImportanceSampleGGX(vec2 xi, vec3 n, float roughness);
float VDCInverse(uint bits);
vec2 Hammersley(uint i, uint sampleCount);

// Implementation
// Main Functions
#ifdef FRAGMENT_SHADER
vec3 GetLighting(const Vertex vert, const PBRFragment frag, const Light light)
{
    switch(light.Type)
    {
        case LIGHT_DIRECTIONAL: return GetLightingDirectional(vert, frag, light);
        default: return vec3(0.0);
    }
}

vec3 GetLightingDirectional(const Vertex vert, const PBRFragment frag, const Light light)
{
    // Vector Setup
    vec3 eye = normalize(Camera.Position - vert.Position);
    vec3 halfEye = normalize(light.Position + eye);

    float nDotV = max(dot(frag.Normal, eye), 0.0);
    float nDotL = max(dot(frag.Normal, light.Position), 0.0);
    float nDotH = max(dot(frag.Normal, halfEye), 0.0);
    float eDotH = clamp(dot(halfEye, eye), 0.0, 1.0);

    // PBR Setup
    vec3 f = FresnelSchlick(frag.F0, eDotH);
    float d = GGXNDF(nDotH, frag.Roughness);
    float g = GSchlickAnalytical(nDotL, nDotV, frag.Roughness);

    vec3 kD = mix(1.0 - f, vec3(0.0), frag.Metallic);

    // Final Calculations
    vec3 specularBRDF = (f * d * g) / max(4.0 * nDotL * nDotV, EPSILON);
    vec3 diffuseBRDF = frag.Albedo * kD;

    // Falloff of nDotL * nDotL is nicer to me
    float lambert = min(nDotL * nDotL, GetShadowDirectional(vert, light));

    return (diffuseBRDF + specularBRDF * frag.Specular) * lambert * light.Color;
}
#endif

#if defined(FRAGMENT_SHADER) && defined(EXT_BINDLESS)
vec3 GetLighting(const Vertex vert, const PBRFragment frag, const PBRSample pbrSample, samplerCube cubemap)
{
    vec3 eye = normalize(Camera.Position - vert.Position);

    float nDotV = clamp(dot(frag.Normal, eye), 0.0, 1.0);
    vec3 f = FresnelSchlick(frag.F0, nDotV);
    vec2 brdf = pbrSample.BRDF;

    // Final Calculations
    vec3 specularColor = textureLod(cubemap, pbrSample.Specular, 0.0).rgb;

    return (brdf.g + f * brdf.r) * specularColor;
}

vec3 GetLighting(const Vertex vert, const PBRFragment frag, const PBRSample pbrSample, const Cubemap cubemap)
{
    // PBR Setup
    vec3 eye = normalize(Camera.Position - vert.Position);

    float nDotV = clamp(dot(frag.Normal, eye), 0.0, 1.0);
    vec3 f = FresnelSchlick(frag.F0, nDotV);
    vec3 r = CubemapParallax(vert.Position, pbrSample.Specular, cubemap);
    vec2 brdf = pbrSample.BRDF;

    // Final Calculations
    vec3 specularColor = textureLod(cubemap.Color, r, 0.0).rgb;

    return (brdf.g + f * brdf.r) * specularColor;
}

vec4 GetLighting(const Vertex vert, const PBRFragment frag, const PBRSample pbrSample, const VoxelGridData grid)
{
    // PBR Setup
    vec3 eye = normalize(Camera.Position - vert.Position);

    float nDotV = clamp(dot(frag.Normal, eye), 0.0, 1.0);
    vec3 f = FresnelSchlick(frag.F0, nDotV);
    vec3 r = pbrSample.Specular;
    vec2 brdf = pbrSample.BRDF;

    Ray ray = Ray(vert.Position, 10.f, r);
    RayResult result = TraceOffset(ray, vert.Normal);

    // Final Calculations
    vec3 specularColor = textureLod(grid.Color, WorldToUV(result.Position), 0.0).rgb;
    specularColor *= brdf.g + f * brdf.r;

    return vec4(specularColor, float(result.Hit));
}
#endif

// PBR Functions
float GGXNDF(float nDotV, float roughness)
{
    roughness = max(roughness, 0.01);
    float alpha = roughness * roughness;
    alpha *= alpha; // Disney reparameterization

    float denom = (nDotV * nDotV) * (alpha - 1.0) + 1.0;
    denom = PI * denom * denom;

    return alpha / max(denom, EPSILON);
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
    float k = (r * r) / 8;
    return GSchlick(nDotL, k) * GSchlick(nDotV, k);
}

vec3 FresnelSchlick(vec3 f0, float nDotV)
{
    return f0 + (1.0 - f0) * pow(1.0 - nDotV, 5.0);
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

    vec3 up = abs(n.z) < (1.0 - EPSILON) ? vec3(0.0, 0.0, 1.0) : vec3(1, 0.0, 0.0);
    vec3 tanX = normalize(cross(up, n));
    vec3 tanY = cross(n, tanX);

    return tanX * h.x + tanY * h.y + n * h.z;
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
vec3 CubemapParallax(vec3 pos, vec3 dir, Cubemap cubemap)
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

            if(any(greaterThan(abs(pos - cubemap.Position), cubemap.Scale))) return dir;
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
    vec3 r = -reflect(eye, n);
    vec3 f = FresnelSchlick(frag.F0, nDotV);
    vec2 brdf = textureLod(BRDFLutTex, vec2(nDotV, frag.Roughness), 0.0).rg;

    return PBRSample(brdf, f, r, vec3(0.0));
}
#endif