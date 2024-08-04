#extension GL_ARB_shader_viewport_layer_array : require

layout(location = 0) in vec3 Position;

#include "Include/Camera.glsl"
#include "Include/Scene.glsl"

struct VertexOut
{
    vec3 FragPos;
    vec4 CurrentNDC;
    vec4 PreviousNDC;
};

out VertexOut VertexIn;

void main()
{
    mat4 viewProjection = Camera.Projection * Camera.View[ViewIndex];

    gl_Position = (viewProjection * vec4(Position, 0.f)).xyww;
    gl_Layer = gl_InstanceID;

    VertexIn.FragPos = Position;
    VertexIn.CurrentNDC = gl_Position;
    VertexIn.PreviousNDC = (Camera.PreviousViewProjection * vec4(Position, 0.f)).xyww;
}