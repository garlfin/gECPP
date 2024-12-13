#include "Include/Camera.glsl"
#include "Include/Scene.glsl"

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec3 Velocity;

struct VertexOut
{
    vec3 FragPos;
    vec4 CurrentNDC;
    vec4 PreviousNDC;
};

in VertexOut VertexIn;

void main()
{
#ifdef EXT_BINDLESS
    FragColor = texture(Lighting.Skybox, VertexIn.FragPos);
#else
    FragColor = vec4(2.0);
#endif

    Velocity = PerspectiveToUV(VertexIn.PreviousNDC);
    Velocity.xy -= PerspectiveToUV(VertexIn.CurrentNDC).xy;
}
