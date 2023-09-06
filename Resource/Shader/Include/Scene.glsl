#define MAX_OBJECT 64

struct SceneData
{
    uint InstanceCount;
    mat4 Model[MAX_OBJECT];
    mat3 Normal[MAX_OBJECT];
};

#ifndef SCENE_UNIFORM_LOCATION
#define SCENE_UNIFORM_LOCATION 0
#endif

#ifndef SCENE_UNIFORM_LAYOUT
#define SCENE_UNIFORM_LAYOUT std140
#endif

layout(SCENE_UNIFORM_LAYOUT, binding = SCENE_UNIFORM_LOCATION) uniform SceneUniform
{
    SceneData Scene;
};

const uint ViewIndex = gl_InstanceID / Scene.InstanceCount;
const uint ModelIndex = gl_InstanceID % Scene.InstanceCount;

