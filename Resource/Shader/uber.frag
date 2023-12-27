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

void main()
{
    if(!bool(Scene.State & ENABLE_COLOR)) return;

    vec3 albedo = texture(AlbedoTex, VertexIn.UV).rgb;
    vec3 amr = texture(AMRTex, VertexIn.UV).rgb;
    vec3 normal = texture(NormalTex, VertexIn.UV * vec2(1, -1)).rgb;

    normal = normalize(VertexIn.TBN * (normal * 2.0 - 1.0));
    if(!gl_FrontFacing) normal *= -1;

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
        bool(Scene.State & ENABLE_SPECULAR) ? 1.0 : 0.0
    );

    FragColor.rgb = albedo * 0.1;
    FragColor.rgb += GetLighting(vertex, fragment, Lighting.Lights[0]);

#ifdef GL_ARB_bindless_texture
    if(bool(Scene.State & ENABLE_SPECULAR))
    {
        for(uint i = 0; i < Lighting.CubemapCount; i++)
            FragColor.rgb += GetLighting(vertex, fragment, Lighting.Cubemaps[i]);
    }
#endif

	Velocity = ((VertexIn.CurrentUV.xy / VertexIn.CurrentUV.w) - (VertexIn.PreviousUV.xy / VertexIn.PreviousUV.w)) * 0.5;

    if(!bool(Scene.State & ENABLE_VOXEL_WRITE)) return;

    Voxel voxel = Voxel(FragColor.rgb, vec4(fragment.Roughness, fragment.Specular, fragment.Metallic, 1.0));
    WriteVoxel(vertex.Position, voxel);
}