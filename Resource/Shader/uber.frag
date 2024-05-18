#define ENABLE_VOXEL_TRACE

#include "Include/Camera.glsl"
#include "Include/Scene.glsl"
#include "Include/Voxel.glsl"
#include "Include/PBR.glsl"
#include "Include/Effect.glsl"

uniform sampler2D AlbedoTex;
uniform sampler2D ARMDTex;
uniform sampler2D NormalTex;

struct VertexOut
{
    vec3 FragPos;
    vec2 UV;
    vec4 PreviousUV;
    vec4 CurrentUV;
    vec4 FragPosLightSpace[MAX_LIGHTS];
    mat3 TBN;
};

in VertexOut VertexIn;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec2 Velocity;

layout(early_fragment_tests) in;
void main()
{
    if(!bool(Scene.State & ENABLE_COLOR)) return;

    Vertex vert = Vertex
    (
        VertexIn.FragPos,
        normalize(VertexIn.TBN[2]),
        VertexIn.FragPosLightSpace[0],
        VertexIn.TBN,
        VertexIn.UV * 10
    );

	ParallaxEffectSettings parallaxSettings = ParallaxEffectSettings(0.5f, 16, 32, 0.0, 0.5);
    vec3 viewDir = normalize(Camera.Position - VertexIn.FragPos);
    vec2 uv = ParallaxMapping(viewDir, ARMDTex, vert, parallaxSettings);

    vec3 albedo = texture(AlbedoTex, uv).rgb;
    vec3 amr = texture(ARMDTex, uv * vec2(1, -1)).rgb;
    vec3 normal = texture(NormalTex, uv).rgb;

	normal = normal * 2.0 - 1.0;
    normal = normalize(VertexIn.TBN * normal);

    PBRFragment frag = PBRFragment
    (
        normal,
        amr.y,
        albedo,
        amr.z,
        vec3(0.04),
        bool(Scene.State & ENABLE_SPECULAR) ? 1.0 : 0.0
    );

    PBRSample pbrSample = ImportanceSample(vert, frag);

    FragColor.rgb = albedo * 0.1 * SS_AO(vert);
    FragColor.rgb += GetLighting(vert, frag, Lighting.Lights[0]);

#ifdef EXT_BINDLESS
    if(bool(Scene.State & ENABLE_SPECULAR))
    {
        vec3 cubemapDir = CubemapParallax(vert.Position, pbrSample.Specular, Lighting.Cubemaps[0]);
        vec3 specular = textureLod(Lighting.Cubemaps[0].Color, cubemapDir, 0.f).rgb;

    #if defined(ENABLE_SS_TRACE) || defined(ENABLE_VOXEL_TRACE)
        Ray ray = Ray(vert.Position, 10.f, pbrSample.Specular);
        RayResult result = SS_Trace(ray);
        vec3 raySpecular = textureLod(Camera.Color, result.Position.xy, 0.f).rgb;

        #ifdef ENABLE_VOXEL_TRACE
            if(!result.Hit)
            {
                ray.Position = vert.Position + ray.Direction * result.Distance;
                result = Voxel_TraceOffset(ray, vert.Normal);
                raySpecular = textureLod(VoxelGrid.Color, Voxel_WorldToUV(result.Position), 0.0).rgb;
                raySpecular = UnpackColor(raySpecular);
            }
        #endif

        if(result.Hit) specular = raySpecular;
    #endif

        FragColor.rgb += FilterSpecular(vert, frag, pbrSample, specular);
    }
#endif

    FragColor.a = 1.0;

	Velocity = ((VertexIn.CurrentUV.xy / VertexIn.CurrentUV.w) - (VertexIn.PreviousUV.xy / VertexIn.PreviousUV.w)) * 0.5;

    if(!bool(Scene.State & ENABLE_VOXEL_WRITE)) return;

    ivec3 texel = Voxel_WorldToTexel(vert.Position, imageSize(VoxelColorOut).x);
    imageStore(VoxelColorOut, texel, PackColor(FragColor));
}