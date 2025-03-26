layout(location = 0) in vec3 Position;

layout(std430, binding = 11) restrict readonly buffer JointsInBuf
{
    mat4 JointsIn[256];
};

#include "Include/Camera.glsl"
#include "Include/Scene.glsl"

void main()
{
    mat4 viewProjection = Camera.Projection * Camera.View[0];
    mat4 model = Scene.Objects[0].Model;

    vec3 position = Position;
    if(gl_VertexID == 1) position.y /= length(model[1].xyz);

    gl_Position = viewProjection * model * JointsIn[gl_InstanceID] * vec4(position, 1.0);
}