layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;

#include "Include/Camera.glsl"
#include "Include/Scene.glsl"

struct VertexOut
{
vec3 FragPos;
vec2 UV;
mat3 TBN;
};

out VertexOut Out;

void main()
{
    mat4 ViewProjection = Camera.Projection * Camera.View[ViewIndex];

    gl_Position = ViewProjection * Scene.Model[ModelIndex] * vec4(Position, 1);
    Out.FragPos = gl_Position.xyz;

    Out.UV = UV;

    vec3 v_Normal, v_Tangent, v_Bitangent;
    v_Normal = normalize(Scene.Normal[ModelIndex] * Normal);
    v_Tangent = normalize(Scene.Normal[ModelIndex] * Tangent);
    v_Bitangent = normalize(cross(v_Tangent, v_Normal));

    Out.TBN = mat3(v_Tangent, v_Bitangent, v_Normal);
}