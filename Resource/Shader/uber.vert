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
	vec4 PreviousUV;
    vec4 CurrentUV;
    vec4 FragPosLightSpace[MAX_LIGHTS];
    mat3 TBN;
};

out VertexOut VertexIn;

void main()
{
    mat4 viewProjection = Camera.Projection * Camera.View[ViewIndex];

    VertexIn.FragPos = (Scene.Model[ModelIndex] * vec4(Position, 1)).xyz;
    VertexIn.UV = UV;

    VertexIn.CurrentUV = viewProjection * Scene.Model[ModelIndex] * vec4(VertexIn.FragPos, 1);

    if(bool(Scene.State & ENABLE_JITTER))
    {
        vec2 jitter = Jitter(Camera.Frame % 16, ivec2(1280, 720));
        gl_Position = JitterMat(jitter) * VertexIn.CurrentUV;
    } else gl_Position = VertexIn.CurrentUV;

    if(!bool(Scene.State & WRITE_MODE_COLOR)) return;

    VertexIn.PreviousUV = Scene.PreviousModel[ModelIndex] * vec4(Position, 1);
    VertexIn.PreviousUV = Camera.PreviousViewProjection * vec4(VertexIn.PreviousUV.xyz, 1);

    vec3 vNormal, vTangent, vBitangent;
    vNormal = normalize(Scene.Normal[ModelIndex] * Normal);
    vTangent = normalize(Scene.Normal[ModelIndex] * Tangent);
    vBitangent = normalize(cross(vTangent, vNormal));

    VertexIn.TBN = mat3(vTangent, vBitangent, vNormal);

    for(uint i = 0; i < Lighting.LightCount; i++)
    	VertexIn.FragPosLightSpace[i] = Lighting.Lights[i].ViewProjection * vec4(VertexIn.FragPos, 1);
}