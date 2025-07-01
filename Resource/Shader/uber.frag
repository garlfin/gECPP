#define ENABLE_SMRT
#define ENABLE_POM
#ifndef VR
    //#define ENABLE_VOXEL_TRACE
    #define ENABLE_SS_TRACE
    #define ENABLE_SMRT_CONTACT_SHADOW
    #define ENABLE_SSAO
    //#define ENABLE_GI
#endif

#define HIZ_MAX_ITER 128

#define POM_MIN_LAYER 8
#define POM_MAX_LAYER 32

#include <Resource/Shader/Include/Scene.glsl>

#include <Resource/Shader/Include/Camera.glsl>
#include <Resource/Shader/Include/PBR.glsl>
#include <Resource/Shader/Include/Effect.glsl>

#ifdef ENABLE_VOXEL_TRACE
#include <Resource/Shader/Include/Voxel.glsl>
#endif

#ifdef ENABLE_SDF_TRACE
#include <Resource/Shader/Include/SDF.glsl>
#endif

struct PBRMaterialData
{
    vec2 Scale;
    vec2 Offset;

    float ParallaxDepth;
    float NormalStrength;
    float AOStrength;

    uint sRGB;

    BINDLESS_TEXTURE(sampler2D, Albedo);
    BINDLESS_TEXTURE(sampler2D, ARMD);
    BINDLESS_TEXTURE(sampler2D, Normal);
};

layout(std140, binding = 4) uniform PBRMaterialUniform
{
    PBRMaterialData Materials[MAX_OBJECTS];
};

#define OBJECT_MATERIAL Materials[ObjectIndexIn]

struct VertexOut
{
    vec3 FragPos;
    vec2 UV;
    vec4 CurrentNDC;
    vec4 PreviousNDC;
    vec4 FragPosSunSpace;
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
        (VertexIn.UV + OBJECT_MATERIAL.Offset) * OBJECT_MATERIAL.Scale
    );

	ParallaxEffectSettings parallaxSettings = ParallaxEffectSettings(OBJECT_MATERIAL.ParallaxDepth, POM_MIN_LAYER, POM_MAX_LAYER, 0.0, 0.5);

    vec3 viewDir = normalize(Camera.Position[ViewIndex] - VertexIn.FragPos);

#if defined(EXT_BINDLESS) && defined(EXT_BINDLESS)
    if(OBJECT_MATERIAL.ParallaxDepth > 0.0)
        vert.UV = ParallaxMapping(viewDir, OBJECT_MATERIAL.ARMD, vert, parallaxSettings);
#endif

#ifdef EXT_BINDLESS
    vec3 albedo = texture(OBJECT_MATERIAL.Albedo, vert.UV).rgb;
    vec3 armd = texture(OBJECT_MATERIAL.ARMD, vert.UV).rgb;
    if((OBJECT_MATERIAL.sRGB & 1) == 1) armd = pow(armd, vec3(1.0 / 2.2));
    vec3 normal = texture(OBJECT_MATERIAL.Normal, vert.UV).rgb;
    if((OBJECT_MATERIAL.sRGB >> 8) == 1) armd = pow(armd, vec3(1.0 / 2.2));
#else
    vec3 albedo = vec3(1.0);
    vec3 armd = vec3(1.0, 1.0, 0.0);
    vec3 normal = vec3(0.5);
#endif
	normal = normal * 2.0 - 1.0;
    normal = normalize(VertexIn.TBN * normal);
    normal = mix(VertexIn.TBN[2], normal, OBJECT_MATERIAL.NormalStrength);
    normal = normalize(normal);

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
    AOSettings aoSettings = AOSettings(8, 0.2, 0.5, 0.5);

    float ao = SS_GetAOStrength(armd.r, OBJECT_MATERIAL.AOStrength);
#if defined(EXT_BINDLESS) && defined(ENABLE_SSAO)
    if(Scene_EnablePostProcess)
        ao *= SS_AO(aoSettings, vert);
#endif

    vec3 ambient = SH_SampleProbe(Lighting.SkyboxIrradiance, frag.Normal).rgb / (2.0 * PI);
#ifdef ENABLE_GI
    ambient = SampleLighting(vert, pbrSample.Diffuse, true);
#endif

#ifdef APPLY_AO_LAST
    FragColor.rgb = albedo * ambient;
#else
    FragColor.rgb = albedo * ambient * ao;
#endif

    FragColor.rgb += GetLighting(vert, frag, Lighting.Sun, VertexIn.FragPosSunSpace);
    for(int i = 0; i < OBJECT_LIGHTING.LightCount; i++)
        FragColor.rgb += GetLighting(vert, frag, OBJECT_LIGHTING.Lights[i], VertexIn.FragPosLightSpace[i]);

    if(Scene_EnableSpecular)
    {
        vec3 specular = SampleLighting(vert, pbrSample.Specular);
        FragColor.rgb += FilterSpecular(vert, frag, pbrSample, specular);
    }

#ifdef APPLY_AO_LAST
    FragColor.rgb *= ao;
#endif

    FragColor.a = 1.0;
    FragColor = FixNan(FragColor);

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
    for(uint i = 0; i < OBJECT_LIGHTING.CubemapCount; i++)
    {
        float weight = 1.0;
        vec3 cubemapDir = CubemapParallax(vert.Position, sampleDirection, OBJECT_LIGHTING.Cubemaps[i], weight);

        cubemapColor += textureLod(OBJECT_LIGHTING.Cubemaps[i].Color, cubemapDir, 0.f).rgb;
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
        SSRaySettings raySettings = SSRaySettings(HIZ_MAX_ITER, EPSILON, 0.05, vert.Normal);

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