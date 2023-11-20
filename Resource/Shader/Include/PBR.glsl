#include "Scene.glsl"
#include "Camera.glsl"
#include "Shadow.glsl"
#include "Vertex.glsl"

#ifndef PI
#define PI 3.141592
#endif
#define EPSILON 0.00001

struct PBRFragment
{
    vec3 Normal;
    float Roughness;
    vec3 Albedo;
    float Metallic;
    vec3 F0;
    float IOR;
};

// Main Functions
vec3 GetLighting(const Vertex vert, const PBRFragment frag, const Light light);
vec3 GetLightingDirectional(const Vertex vert, const PBRFragment frag, const Light light);

// PBR Functions
// https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float GGXNDF(float nDotV, float roughness);
float GSchlick(float cosTheta, float roughness); // AKA 'k'
float GSchlick(float nDotL, float nDotH, float roughness);
float GSchlickAnalytical(float nDotL, float nDotH, float roughness); // Read more: Section 3, "Specular G"
vec3 FresnelSchlick(vec3 f0, float nDotV);

// Implementation
// Main Functions
vec3 GetLighting(const Vertex vert, const PBRFragment frag, const Light light)
{
    // No control flow indirection because it's all in uniforms
    switch(light.Type)
    {
        case LIGHT_DIRECTIONAL: return GetLightingDirectional(vert, frag, light);
        default: return vec3(1.0);
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

    // PBR Setup
    vec3 f = FresnelSchlick(frag.F0, max(dot(nDotH, nDotV), 0.0));
    float d = GGXNDF(nDotH, frag.Roughness);
    float g = GSchlickAnalytical(nDotL, nDotV, frag.Roughness);

    vec3 kD = mix(1.0 - f, vec3(0.0), frag.Metallic);

    // Final Calculations
    vec3 specularBRDF = (f * d * g) / max(4.0 * nDotL * nDotV, EPSILON);
    vec3 diffuseBRDF = frag.Albedo; // * kD; Maybe a bug? Odd darkness around the edges.

    // Falloff of nDotL * nDotL is nicer to me
    float lambert = min(nDotL * nDotL, GetShadowDirectional(vert, light));

    return (diffuseBRDF + specularBRDF) * lambert * light.Color;
}

// PBR Functions
float GGXNDF(float nDotV, float roughness)
{
    float alpha = roughness * roughness;
    alpha *= alpha; // Disney reparameterization

    float denom = (nDotV * nDotV) * (alpha - 1.0) + 1.0;
    return alpha / (PI * denom * denom);
}

float GSchlick(float cosTheta, float roughness)
{
    return cosTheta / (cosTheta * (1.0 - roughness) + roughness);
}

float GSchlick(float nDotL, float nDotH, float roughness)
{
    float k = roughness / 2;
    return GSchlick(nDotL, k) * GSchlick(nDotH, k);
}

float GSchlickAnalytical(float nDotL, float nDotH, float roughness)
{
    float r = roughness + 1;
    float k = (r * r) / 8;
    return GSchlick(nDotL, k) * GSchlick(nDotH, k);
}

vec3 FresnelSchlick(vec3 f0, float nDotV)
{
    return f0 + (1.0 - f0) * pow(1.0 - nDotV, 5.0);
}