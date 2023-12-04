#include "Include/Camera.glsl"
#include "Include/Scene.glsl"

in vec3 FragPos;
out vec4 FragColor;

void main()
{
#ifdef EXT_BINDLESS
    FragColor = texture(Lighting.Skybox, FragPos);
#else
    FragColor = vec4(1.0);
#endif
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / 2.2));
}
