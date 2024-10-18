#extension GL_ARB_shader_viewport_layer_array : require

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;

#include "Include/Camera.glsl"
#include "Include/Scene.glsl"
#include "Include/Shadow.glsl"
#include "Include/TAA.glsl"

struct VertexOut
{
    vec3 FragPos;
    vec2 UV;
    vec4 CurrentNDC;
	vec4 PreviousNDC;
    vec4 FragPosLightSpace[MAX_LIGHTS];
    mat3 TBN;
};

out VertexOut VertexIn;

void main()
{
    mat4 viewProjection = Camera.Projection * Camera.View[ViewIndex];

    ObjectInfo objectInfo = Scene.Objects[ObjectIndex];

    VertexIn.FragPos = (objectInfo.Model * vec4(Position, 1)).xyz;
    VertexIn.UV = UV;

    gl_Position = viewProjection * vec4(VertexIn.FragPos, 1);
    gl_Layer = int(ViewIndex);
    VertexIn.CurrentNDC = gl_Position;

    if(ENABLE_JITTER)
    {
        vec2 jitter = Jitter(Camera.Frame, Camera.Size);
        gl_Position.xy += jitter * gl_Position.w;
    }

    if(!ENABLE_COLOR) return;

    VertexIn.PreviousNDC = objectInfo.PreviousModel * vec4(Position, 1);
    VertexIn.PreviousNDC = Camera.PreviousViewProjection * vec4(VertexIn.PreviousNDC.xyz, 1);

    vec3 nor, tan, bitan;
    nor = normalize(objectInfo.Normal * Normal);
    tan = normalize(objectInfo.Normal * Tangent);
    bitan = normalize(cross(nor, tan));

    VertexIn.TBN = mat3(tan, bitan, nor);

    for(uint i = 0; i < Lighting.LightCount; i++)
        VertexIn.FragPosLightSpace[i] = Lighting.Lights[i].ViewProjection * vec4(VertexIn.FragPos, 1);
}