#include "Include/Camera.glsl"
#include "Include/Scene.glsl"

in vec3 FragPos;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec2 Velocity;

void main()
{
#ifdef EXT_BINDLESS
    FragColor = texture(Lighting.Skybox, FragPos);
#else
    FragColor = vec4(1.0);
#endif

	Velocity = vec2(0.0);
}
