#define ENABLE_VOXEL_TRACE

#include "Include/Camera.glsl"
#include "Include/Scene.glsl"
#include "Include/Voxel.glsl"
#include "Include/PBR.glsl"

uniform sampler2D AlbedoTex;
uniform sampler2D AMRTex;
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

    vec3 albedo = texture(AlbedoTex, VertexIn.UV).rgb;
    vec3 amr = texture(AMRTex, VertexIn.UV).rgb;
    vec3 normal = texture(NormalTex, VertexIn.UV * vec2(1, -1)).rgb;

    normal = normalize(VertexIn.TBN * (normal * 2.0 - 1.0));

    Vertex vert = Vertex
    (
        VertexIn.FragPos,
        normalize(VertexIn.TBN[2]),
        VertexIn.FragPosLightSpace[0]
    );

    PBRFragment frag = PBRFragment
    (
        normal,
        amr.b,
        albedo,
        amr.g,
        vec3(0.04),
        bool(Scene.State & ENABLE_SPECULAR) ? 1.0 : 0.0
    );

    PBRSample pbrSample = ImportanceSample(vert, frag);

    FragColor.rgb = albedo * 0.1;
    FragColor.rgb += GetLighting(vert, frag, Lighting.Lights[0]);

#ifdef EXT_BINDLESS
    if(bool(Scene.State & ENABLE_SPECULAR))
    {
        vec3 specular = GetLighting(vert, frag, pbrSample, Lighting.Cubemaps[0]);

    #ifdef ENABLE_VOXEL_TRACE
        vec4 voxelSpecular = GetLighting(vert, frag, pbrSample, VoxelGrid);
        specular = mix(specular, voxelSpecular.rgb, voxelSpecular.a);
    #endif

        FragColor.rgb += specular;
    }
#endif

    FragColor.a = 1.0;

	Velocity = ((VertexIn.CurrentUV.xy / VertexIn.CurrentUV.w) - (VertexIn.PreviousUV.xy / VertexIn.PreviousUV.w)) * 0.5;

    if(!bool(Scene.State & ENABLE_VOXEL_WRITE)) return;

    ivec3 texel = WorldToTexel(vert.Position, imageSize(VoxelColorOut).x);
    imageStore(VoxelColorOut, texel, PackColor(FragColor));
}