#include "Include/Camera.glsl"
#include "Include/Scene.glsl"
#include "Include/Voxel.glsl"

uniform sampler2D Albedo;

struct VertexOut
{
    vec3 FragPos;
    vec2 UV;
    mat3 TBN;

    vec3 FragPosLightSpace[MAX_LIGHTS];
};

in VertexOut VertexIn;

// TODO: FIX THIS! !
#include "Include/PBR.glsl"

out vec4 FragColor;

void main()
{
    if(Scene.Stage == STAGE_PRE_Z) return;

    vec3 albedo = texture(Albedo, VertexIn.UV).rgb;
    FragColor.rgb = albedo * 0.1;

    Vertex vertex = Vertex
    (
        VertexIn.FragPos,
        normalize(VertexIn.TBN[2]),
        VertexIn.FragPosLightSpace[0]
    );

    PBRFragment fragment = PBRFragment
    (
        normalize(VertexIn.TBN[2]),
        0.1,
        albedo,
        0.0,
        vec3(0.04),
        1.46
    );

    FragColor.rgb += GetLighting(vertex, fragment, Lighting.Lights[0]);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / 2.2));
    if(Scene.Stage != STAGE_VOXEL) return;

    Voxel voxel = Voxel(FragColor.rgb, vec3(0), true);
    //WriteVoxel(Vertex.FragPos, voxel);
}