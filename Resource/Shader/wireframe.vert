layout(location = 0) in vec3 Position;

#include "Include/Camera.glsl"
#include "Include/Scene.glsl"

void main()
{
    mat4 viewProjection = Camera.Projection * Camera.View[0];
    mat4 model = Scene.Objects[0].Model;

    vec3 position = Position;

    gl_Position = viewProjection * model * vec4(position, 1.0);
}