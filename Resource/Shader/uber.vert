layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec3 Normal;
layout(location = 3) in vec3 Tangent;

#include "Include/Camera.glsl"
#include "Include/Scene.glsl"
#include "Include/Shadow.glsl"

struct VertexOut
{
    vec3 FragPos;
    vec2 UV;
    vec4 FragPosLightSpace[MAX_LIGHTS];
	vec4 CurrentPosition;
	vec4 PreviousPosition;
    mat3 TBN;
};

out VertexOut VertexIn;

void main()
{
    mat4 viewProjection = Camera.Projection * Camera.View[ViewIndex];

    VertexIn.FragPos = (Scene.Model[ModelIndex] * vec4(Position, 1)).xyz;
    VertexIn.UV = UV;

    gl_Position = viewProjection * Scene.Model[ModelIndex] * vec4(VertexIn.FragPos, 1);

    if(Scene.Stage == STAGE_PRE_Z) return;

    VertexIn.PreviousPosition = Scene.PreviousModel[ModelIndex] * vec4(Position, 1);
    VertexIn.PreviousPosition = Camera.PreviousViewProjection * vec4(VertexIn.PreviousPosition.xyz, 1);
	VertexIn.CurrentPosition = gl_Position;

    vec3 vNormal, vTangent, vBitangent;
    vNormal = normalize(Scene.Normal[ModelIndex] * Normal);
    vTangent = normalize(Scene.Normal[ModelIndex] * Tangent);
    vBitangent = normalize(cross(vTangent, vNormal));

    VertexIn.TBN = mat3(vTangent, vBitangent, vNormal);

    for(uint i = 0; i < Lighting.LightCount; i++)
    	VertexIn.FragPosLightSpace[i] = Lighting.Lights[i].ViewProjection * vec4(VertexIn.FragPos, 1);
}