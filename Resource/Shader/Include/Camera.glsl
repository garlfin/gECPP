struct CameraData
{
    vec3 Position;
    vec2 ClipPlanes;
    float FOV;
    float _pad;

    mat4 Projection;
    mat4 PreviousViewProjection;
    mat4 View[6];
};

#ifndef CAMERA_UNIFORM_LOCATION
#define CAMERA_UNIFORM_LOCATION 1
#endif

#ifndef CAMERA_UNIFORM_LAYOUT
#define CAMERA_UNIFORM_LAYOUT std140
#endif

layout(CAMERA_UNIFORM_LAYOUT, binding = CAMERA_UNIFORM_LOCATION) uniform CameraUniform
{
    CameraData Camera;
};

