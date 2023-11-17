#include "Scene.glsl"

struct PBRFragment
{
    vec3 Position;
    float Roughness;
    vec3 Normal;
    float Metallic;
    vec3 Albedo;
    float IOR;
};

vec3 GetLighting(inout PBRFragment frag, uint index);
vec3 GetLightingDirectional(inout PBRFragment frag, uint index);

vec3 GetLighting(inout PBRFragment frag, uint index)
{
    switch(Lighting.Lights[index].Type)
    {
        case LIGHT_DIRECTIONAL: return GetLightingDirectional(frag, index);
        default: return vec3(1);
    }
}

vec3 GetLightingDirectional(inout PBRFragment frag, uint index)
{
    const Light light = Lighting.Lights[index];
    const vec3 lightCoord = Vertex.FragPosLightSpace[index];
    const vec3 lightDir = normalize(light.Position);

    float lambert = max(dot(frag.Normal, lightDir), 0);

#ifdef GL_ARB_bindless_texture
    float shadow = lightCoord.z - texture(sampler2D(light.Depth), lightCoord.xy).r;
#else
    // TODO: THIS!!!
    float shadow = 0;
#endif
    shadow = shadow > 0.003 ? 0 : 1;

    return frag.Albedo * min(shadow, lambert);
}