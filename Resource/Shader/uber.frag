#include "Include/Camera.glsl"
#include "Include/Voxel.glsl"

uniform sampler2D Albedo;

struct VertexOut
{
    vec3 FragPos;
    vec2 UV;
    mat3 TBN;
};

in VertexOut Vertex;

out vec4 FragColor;

void main()
{
    if(Camera.Stage == STAGE_PRE_Z) return;

    vec3 viewDir = normalize(Vertex.FragPos - Camera.Position);
    float lambert = dot(Vertex.TBN[2], normalize(vec3(1, 1, 1))) * 0.5 + 0.5;
    FragColor = texture(Albedo, Vertex.UV);
    FragColor *= lambert;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / 2.2));

    if(Camera.Stage != STAGE_VOXEL) return;

    Voxel voxel = Voxel(FragColor.rgb, vec3(0), true);
    //WriteVoxel(Vertex.FragPos, voxel);
}