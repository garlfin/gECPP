#include "Scene.glsl"

struct PBRFragment
{
    vec3 Position;
    float IOR;
    vec3 PositionLightSpace;
    float Roughness;
    vec3 CameraDir;
    float Metallic;
};

vec3 GetLighting(inout PBRFragment frag, Light light);
vec3 GetLightingDirectional(inout PBRFragment frag, Light light);

vec3 GetLighting(inout PBRFragment frag, Light light)
{
    switch(light.Type)
    {
        case LIGHT_DIRECTIONAL: return GetLightingDirectional(frag, light);
        default: return vec3(1);
    }
}

vec3 GetLightingDirectional(inout PBRFragment frag, Light light)
{
    vec3 lightCoord = frag.PositionLightSpace;

#ifdef GL_ARB_bindless_texture
    float shadowDelta = lightCoord.z - texture(sampler2D(light.Depth), lightCoord.xy).r;
#else
    float shadowDelta = 0;
#endif

    return shadowDelta > 0.003 ? vec3(0) : vec3(1);
}