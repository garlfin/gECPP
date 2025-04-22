#define ENABLE_SMRT_CONTACT_SHADOW
#define ENABLE_SS_TRACE
#define DIRECTIONAL_SHADOW_RADIUS 0.05

#define HIZ_MAX_ITER 128

#define POM_MIN_LAYER 8
#define POM_MAX_LAYER 32

#include <Resource/Shader/Include/Camera.glsl>
#include <Resource/Shader/Include/Scene.glsl>
#include <Resource/Shader/Include/PBR.glsl>
#include <Resource/Shader/Include/Effect.glsl>

#ifdef ENABLE_VOXEL_TRACE
#include <Resource/Shader/Include/Voxel.glsl>
#endif

#ifdef ENABLE_SDF_TRACE
#include <Resource/Shader/Include/SDF.glsl>
#endif

uniform sampler2D AlbedoTex;

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

vec3 SampleLighting(Vertex vert, vec3 dir);

layout(early_fragment_tests) in;
void main()
{
    if(!bool(Scene_WriteMode & WRITE_MODE_COLOR)) return;

    Vertex vert = Vertex
    (
        VertexIn.FragPos,
        normalize(VertexIn.TBN[2]),
        VertexIn.TBN,
        VertexIn.UV
    );

    vec3 albedo = texture(AlbedoTex, vert.UV).rgb;

    PBRFragment frag = PBRFragment
    (
        VertexIn.TBN[2],
        0.7, // roughness
        albedo,
        0.0, // metallic
        vec3(0.04),
        Scene_EnableSpecular ? 1.0 : 0.0
    );

    PBRSample pbrSample = ImportanceSample(vert, frag);
    AOSettings aoSettings = AOSettings(8, 0.2, 0.5, 0.5);

#ifdef EXT_BINDLESS
    float ao = SS_AO(aoSettings, vert);
#else
    float ao = 1.0;
#endif

    vec3 ambient = SH_SampleProbe(Lighting.SkyboxIrradiance, frag.Normal).rgb;
    FragColor.rgb = albedo * ambient * ao;

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
}

vec3 SampleLighting(Vertex vert, vec3 sampleDirection)
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

    Ray ray = Ray(vert.Position, 10.f, sampleDirection, 0);
    RayResult result = DefaultRayResult;
    vec3 rayColor;

    #ifdef ENABLE_SS_TRACE
    if(Scene_VoxelWriteMode != VOXEL_MODE_WRITE)
    {
        SSRaySettings raySettings = SSRaySettings(HIZ_MAX_ITER, EPSILON, 0.2, vert.Normal);
        SSRay ssRay = CreateSSRayHiZ(ray, raySettings);
        result = SS_Trace(ssRay, raySettings);

        rayColor = textureLod(Camera.Color, result.Position.xy, 0.0).rgb;
    }
    #endif

    if(result.Result == RAY_RESULT_HIT)
        color = rayColor;

    return color;
#endif
}
