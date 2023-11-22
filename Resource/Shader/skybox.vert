layout(location = 0) in vec3 Position;

#include "Include/Camera.glsl"
#include "Include/Scene.glsl"

out vec3 FragPos;

void main()
{
    mat4 ViewProjection = Camera.Projection * mat4(mat3(Camera.View[ViewIndex]));

    FragPos = (Scene.Model[ModelIndex] * vec4(Position, 1)).xyz;

    gl_Position = (ViewProjection * Scene.Model[ModelIndex] * vec4(FragPos, 1)).xyww;
}