#include "Include/Camera.glsl"
#include "Include/Scene.glsl"

in vec3 FragPos;
out vec4 FragColor;

void main() {
    FragColor = texture(Lighting.Skybox, FragPos);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / 2.2));
}
