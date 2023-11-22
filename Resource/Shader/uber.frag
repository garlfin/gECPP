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
    mat3 TBN;

    vec3 FragPosLightSpace[MAX_LIGHTS];
};

in VertexOut VertexIn;

out vec4 FragColor;

void main()
{
    if(Scene.Stage == STAGE_PRE_Z) return;

    vec3 albedo = texture(AlbedoTex, VertexIn.UV).rgb;
    vec3 amr = texture(AMRTex, VertexIn.UV).rgb;
    vec3 normal = texture(NormalTex, VertexIn.UV * vec2(1, -1)).rgb;

    normal = normalize(VertexIn.TBN * (normal * 2.0 - 1.0));

    FragColor.rgb = albedo * 0.1;

    Vertex vertex = Vertex
    (
        VertexIn.FragPos,
        normalize(VertexIn.TBN[2]),
        VertexIn.FragPosLightSpace[0]
    );

    PBRFragment fragment = PBRFragment
    (
        normal,
        amr.b,
        albedo,
        amr.g,
        vec3(0.04),
        1.46
    );

    FragColor.rgb += GetLighting(vertex, fragment, Lighting.Lights[0]);
    FragColor.rgb += GetLighting(vertex, fragment, Lighting.Skybox);

    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / 2.2));

    if(Scene.Stage != STAGE_VOXEL) return;

    Voxel voxel = Voxel(FragColor.rgb, vec3(0), true);
    //WriteVoxel(Vertex.FragPos, voxel);
}