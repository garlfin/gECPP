layout(location = 0) in vec3 Position;

layout(std430, binding = 11) restrict readonly buffer JointsInBuf
{
    mat4 JointsIn[256];
};

#include "Include/Camera.glsl"

void main()
{
    mat4 viewProjection = Camera.Projection * Camera.View[0];
    gl_Position = viewProjection * JointsIn[gl_InstanceID] * vec4(Position, 1.0);
}