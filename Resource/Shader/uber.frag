#define ENABLE_VOXEL_TRACE
//#define ENABLE_SS_TRACE
#define ENABLE_SMRT
//#define ENABLE_SMRT_CONTACT_SHADOW
//#define ENABLE_GI

#define HIZ_MAX_ITER 128

#define POM_MIN_LAYER 8
#define POM_MAX_LAYER 32

#include "Include/Camera.glsl"
#include "Include/Scene.glsl"
#include "Include/PBR.glsl"
#include "Include/Effect.glsl"

#ifdef ENABLE_VOXEL_TRACE
#include "Include/Voxel.glsl"
#endif

#ifdef ENABLE_SDF_TRACE
#include "Include/SDF.glsl"
#endif

uniform sampler2D AlbedoTex;
uniform sampler2D ARMDTex;
uniform sampler2D NormalTex;

struct VertexOut
{
    vec3 FragPos;
    vec2 UV;
    vec4 CurrentNDC;
    vec4 PreviousNDC;
    vec4 FragPosLightSpace[MAX_LIGHTS];
    mat3 TBN;
};

in VertexOut VertexIn;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec3 Velocity;

vec3 SampleLighting(Vertex vert, vec3 dir, bool rough = false);

layout(early_fragment_tests) in;
void main()
{
    if(!bool(Scene_WriteMode & WRITE_MODE_COLOR)) return;

    Vertex vert = Vertex
    (
        VertexIn.FragPos,
        normalize(VertexIn.TBN[2]),
        VertexIn.TBN,
        VertexIn.UV * 10
    );

	ParallaxEffectSettings parallaxSettings = ParallaxEffectSettings(0.5f, POM_MIN_LAYER, POM_MAX_LAYER, 0.0, 0.5);

    vec3 viewDir = normalize(Camera.Position - VertexIn.FragPos);
    vec2 uv = ParallaxMapping(viewDir, ARMDTex, vert, parallaxSettings);

    vec3 albedo = texture(AlbedoTex, uv).rgb;
    vec3 armd = texture(ARMDTex, uv * vec2(1, -1)).rgb;
    vec3 normal = texture(NormalTex, uv).rgb;

	normal = normal * 2.0 - 1.0;
    normal = normalize(VertexIn.TBN * normal);

    PBRFragment frag = PBRFragment
    (
        normal,
        armd.g, // roughness
        albedo,
        armd.b, // metallic
        vec3(0.04),
        Scene_EnableSpecular ? 1.0 : 0.0
    );

    PBRSample pbrSample = ImportanceSample(vert, frag);

    vec3 ambient = SH_SampleProbe(Lighting.SkyboxIrradiance, frag.Normal).rgb / PI;
#ifdef ENABLE_GI
    ambient = SampleLighting(vert, pbrSample.Diffuse, true);
#endif

    FragColor.rgb = albedo * ambient;// * armd.r;

    for(int i = 0; i < Lighting.LightCount; i++)
        FragColor.rgb += GetLighting(vert, frag, Lighting.Lights[i], VertexIn.FragPosLightSpace[i]);

    if(Scene_EnableSpecular)
    {
        vec3 specular = SampleLighting(vert, pbrSample.Specular);
        FragColor.rgb += FilterSpecular(vert, frag, pbrSample, specular);
    }

    FragColor.a = 1.0;

    Velocity = PerspectiveToUV(VertexIn.PreviousNDC);
    Velocity.xy -= PerspectiveToUV(VertexIn.CurrentNDC).xy;

    if(Scene_VoxelWriteMode != VOXEL_MODE_WRITE) return;

    ivec3 texel = Voxel_WorldToTexel(vert.Position, imageSize(VoxelColorOut).x);
    imageStore(VoxelColorOut, texel, Voxel_PackColor(FragColor));
}

vec3 SampleLighting(Vertex vert, vec3 sampleDirection, bool rough)
{
#ifndef EXT_BINDLESS
    return vec3(0.0);
#else
    vec3 color = textureLod(Lighting.Skybox, sampleDirection, 0.0).rgb * PI;

    vec3 cubemapColor;
    float cubemapWeight, maxCubemapWeight;
    for(uint i = 0; i < Lighting.CubemapCount; i++)
    {
        float weight = 1.0;
        vec3 cubemapDir = CubemapParallax(vert.Position, sampleDirection, Lighting.Cubemaps[i], weight);

        cubemapColor += textureLod(Lighting.Cubemaps[i].Color, cubemapDir, 0.f).rgb;
        cubemapWeight += weight;
        maxCubemapWeight = max(maxCubemapWeight, weight);
    }

    color = mix(color, cubemapColor / max(cubemapWeight, EPSILON), maxCubemapWeight);

#if defined(ENABLE_SS_TRACE) || defined(ENABLE_VOXEL_TRACE)
    Ray ray = Ray(vert.Position, 10.f, sampleDirection, 0);
    RayResult result = DefaultRayResult;
    vec3 rayColor;
#endif

#ifdef ENABLE_SS_TRACE
    if(Scene_VoxelWriteMode != VOXEL_MODE_WRITE && !rough)
    {
        SSRaySettings raySettings = SSRaySettings(HIZ_MAX_ITER, EPSILON, 0.2, vert.Normal);

        if(rough)
        {
            SSLinearRaySettings linearSettings = SSLinearRaySettings(raySettings, 0.1, 1.0);
            SSRay ssRay = CreateSSRayLinear(ray, linearSettings);
            result = SS_TraceRough(ssRay, linearSettings);
        }
        else
        {
            SSRay ssRay = CreateSSRayHiZ(ray, raySettings);
            result = SS_Trace(ssRay, raySettings);
        }

        rayColor = textureLod(Camera.Color, result.Position.xy, 0.0).rgb;
    }
#endif

#ifdef ENABLE_VOXEL_TRACE
    if(result.Result != RAY_RESULT_HIT)
    {
        ray.Position = vert.Position + ray.Direction * result.Distance;
        ray.BaseMip = 0;
        result = Voxel_TraceOffset(ray, vert.Normal);
        rayColor = textureLod(VoxelGrid.Color, Voxel_WorldToUV(result.Position), 0.f).rgb;
        rayColor = Voxel_UnpackColor(rayColor);
    }
#endif

#if defined(ENABLE_SS_TRACE) || defined(ENABLE_VOXEL_TRACE)
    if(result.Result == RAY_RESULT_HIT) color = rayColor;
#endif

    return color;
#endif
}
