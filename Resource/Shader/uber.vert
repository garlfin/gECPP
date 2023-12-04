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
    vec2 Velocity;
    vec3 FragPosLightSpace[MAX_LIGHTS];
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

    vec3 previousFragPos = (Scene.PreviousModel[ModelIndex] * vec4(Position, 1)).xyz;
    vec4 previousGLPosition = Camera.PreviousViewProjection * vec4(previousFragPos, 1);

    vec2 newNDC = (gl_Position.xy / gl_Position.w) * 0.5 + 0.5;
    vec2 oldNDC = (previousGLPosition.xy / previousGLPosition.w) * 0.5 + 0.5;

    VertexIn.Velocity = newNDC - oldNDC;

    vec3 vNormal, vTangent, vBitangent;
    vNormal = normalize(Scene.Normal[ModelIndex] * Normal);
    vTangent = normalize(Scene.Normal[ModelIndex] * Tangent);
    vBitangent = normalize(cross(vTangent, vNormal));

    VertexIn.TBN = mat3(vTangent, vBitangent, vNormal);

    for(uint i = 0; i < Lighting.LightCount; i++)
    {
        vec4 lightPos = Lighting.Lights[i].ViewProjection * vec4(VertexIn.FragPos, 1);
        VertexIn.FragPosLightSpace[i] = lightPos.xyz / lightPos.w;
        VertexIn.FragPosLightSpace[i] = VertexIn.FragPosLightSpace[i] * 0.5 + 0.5;
        VertexIn.FragPosLightSpace[i].z = LinearizeDepthOrtho(VertexIn.FragPosLightSpace[i].z, Lighting.Lights[i].Planes);
    }
}