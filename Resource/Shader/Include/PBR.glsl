#include "Scene.glsl"
#include "Camera.glsl"

struct PBRFragment
{
    vec3 Position;
    float Roughness;
    vec3 Normal;
    float Metallic;
    vec3 Albedo;
    float IOR;
};

vec3

bool TexcoordOutOfBounds(vec2 uv) { return any(greaterThan(uv.xy, vec2(1))) || any(lessThan(uv.xy, vec2(0))); }
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
    const vec3 viewDir = normalize(frag.Position - Camera.Position);

    float lambert = dot(frag.Normal, lightDir);
    float bias = mix(0.001, 0.0005, abs(lambert));
    float spec = max(dot(reflect(lightDir, frag.Normal), viewDir), 0.0);

    spec = pow(spec, pow(2 - frag.Roughness, 8.0));
    lambert = max(lambert, 0);
    lambert *= lambert; // I just thought squaring it looked good.

#ifdef EXT_BINDLESS
    float shadow = lightCoord.z - texture(sampler2D(light.Depth), lightCoord.xy).r;
#else
    // TODO: THIS!!!
    float shadow = 0;
#endif
    shadow = shadow > bias ? 0 : 1;
    shadow = TexcoordOutOfBounds(lightCoord.xy) ? 1.0 : shadow;

    lambert = min(shadow, lambert);

    // Yes, I know I don't need the parenthases, no I don't care. It separates the terms.
    return (frag.Albedo * lambert * light.Color) + (spec * light.Color * lambert);
}