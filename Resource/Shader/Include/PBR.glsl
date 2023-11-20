#include "Scene.glsl"
#include "Camera.glsl"
#include "Shadow.glsl"
#include "Fragment.glsl"

struct PBRFragment
{
    Fragment Frag;
    vec3 Albedo;
    float Roughness;
    float Metallic;
    float IOR;
};

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

vec3 GetLightingDirectional(inout PBRFragment pbrFrag, uint index)
{
    const Light light = Lighting.Lights[index];
    Fragment frag = pbrFrag.Frag;

    vec3 lightCoord = Vertex.FragPosLightSpace[index];
    vec3 lightDir = normalize(light.Position);
    vec3 viewDir = normalize(frag.Frag.Position - Camera.Position);

    float lambert = dot(frag.Normal, lightDir);
    float bias = mix(0.005, 0.001, abs(lambert));
    float spec = max(dot(reflect(lightDir, frag.Frag.Normal), viewDir), 0.0);

    spec = pow(spec, pow(2 - pbrFrag.Roughness, 8.0));
    lambert = max(lambert, 0.0);
    lambert = min(GetShadowDirectional(frag, ), lambert);

    // Yes, I know I don't need the parenthases, no I don't care. It separates the terms.
    return (frag.Albedo * lambert * light.Color) + (spec * light.Color * lambert);
}