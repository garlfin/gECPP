#define ENABLE_VOXEL_TRACE
#define ENABLE_SMRT
#define ENABLE_SMRT_CONTACT_SHADOW

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

layout(early_fragment_tests) in;
void main()
{
    if(!ENABLE_COLOR) return;

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
    vec3 arm = texture(ARMDTex, uv * vec2(1, -1)).rgb;
    vec3 normal = texture(NormalTex, uv).rgb;

	normal = normal * 2.0 - 1.0;
    normal = normalize(VertexIn.TBN * normal);

    PBRFragment frag = PBRFragment
    (
        normal,
        arm.y, // roughness
        albedo,
        arm.z,
        vec3(0.04),
        ENABLE_SPECULAR ? 1.0 : 0.0
    );

    PBRSample pbrSample = ImportanceSample(vert, frag);

    FragColor.rgb = albedo * 0.1;// * SS_AO(vert);

    for(int i = 0; i < Lighting.LightCount; i++)
        FragColor.rgb += GetLighting(vert, frag, Lighting.Lights[i], VertexIn.FragPosLightSpace[i]);

#ifdef EXT_BINDLESS
    if(ENABLE_SPECULAR)
    {
        vec3 specular = textureLod(Lighting.Skybox, pbrSample.Specular, 0.0).rgb;
        vec3 cubemapSpecular;

        float cubemapWeight, maxCubemapWeight;
        for(uint i = 0; i < Lighting.CubemapCount; i++)
        {
            float weight;
            vec3 cubemapDir = CubemapParallax(vert.Position, pbrSample.Specular, Lighting.Cubemaps[i], weight);

            cubemapSpecular += textureLod(Lighting.Cubemaps[0].Color, cubemapDir, 0.f).rgb;
            cubemapWeight += weight;
            maxCubemapWeight = max(maxCubemapWeight, weight);
        }

        specular = mix(specular, cubemapSpecular / max(cubemapWeight, EPSILON), maxCubemapWeight);

    #if defined(ENABLE_SS_TRACE) || defined(ENABLE_VOXEL_TRACE)
        Ray ray = Ray(vert.Position, 10.f, pbrSample.Specular);

        SSRaySettings raySettings = SSRaySettings(HIZ_MAX_ITER, EPSILON, 0.2, vert.Normal, 0.0);
        SSRay ssRay = CreateSSRayHiZ(ray, raySettings);

        RayResult result = SS_Trace(ssRay, raySettings);
        vec3 raySpecular = textureLod(Camera.Color, result.Position.xy, 0.f).rgb;

        #ifdef ENABLE_VOXEL_TRACE
            if(result.Result != RAY_RESULT_HIT)
            {
                ray.Position = vert.Position + ray.Direction * result.Distance;
                result = Voxel_TraceOffset(ray, vert.Normal);
                raySpecular = textureLod(VoxelGrid.Color, Voxel_WorldToUV(result.Position), 0.0).rgb;
                raySpecular = UnpackColor(raySpecular);
            }
        #endif

        if(result.Result == RAY_RESULT_HIT) specular = raySpecular;
    #endif

        FragColor.rgb += FilterSpecular(vert, frag, pbrSample, specular);
    }
#endif

    FragColor.a = 1.0;

    Velocity = PerspectiveToUV(VertexIn.PreviousNDC);
    Velocity.xy -= PerspectiveToUV(VertexIn.CurrentNDC).xy;

    if(!ENABLE_VOXEL_WRITE) return;

    ivec3 texel = Voxel_WorldToTexel(vert.Position, imageSize(VoxelColorOut).x);
    imageStore(VoxelColorOut, texel, PackColor(FragColor));
}