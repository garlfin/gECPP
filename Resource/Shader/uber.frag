#include "Include/Camera.glsl"
#include "Include/Voxel.glsl"

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
    vec3 viewDir = normalize(Vertex.FragPos - Camera.Position);
    FragColor = (dot(Vertex.TBN[2], normalize(vec3(1, 1, 1))) * 0.5 + 0.5).rrrr;
    FragColor *= FragColor;

    if(Camera.Stage != STAGE_VOXEL) return;

    Voxel voxel = Voxel(FragColor.rgb, 0, 0, true);
    WriteVoxel(Vertex.FragPos, voxel);

}