#include <Resource/Shader/Include/Camera.glsl>

layout(location = 0) in vec2 Position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec4 Color;

struct VertexOut
{
    vec2 UV;
    vec4 Color;
};

out VertexOut VertexIn;

void main()
{
    VertexIn.UV = UV;
    VertexIn.Color = Color;

    gl_Position = Camera.Projection * vec4(Position, 0.0, 1.0);
    gl_Position.y = -gl_Position.y;
}
