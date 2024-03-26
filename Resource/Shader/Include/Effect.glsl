#include "Bindless.glsl"

struct ParallaxEffectSettings
{
    sampler2D HeightTexture;
    float Depth;
    int MinLayers;
    int MaxLayers;
};

vec2 ParallaxMapping(sampler2D, vec2, vec3, ParallaxEffectSettings);

// Implementation
vec2 ParallaxMapping(vec2 texCoord, vec3 viewDir, ParallaxEffectSettings s)
{
    s.Depth *= 0.1;
    float NdotV = 1.0 - abs(dot(vec3(0.0, 0.0, 1.0), viewDir));
    int layerCount = int(mix(s.MinLayers, s.MaxLayers, NdotV));

    vec3 delta = vec3(viewDir.xy / viewDir.z, -1.0) / layerCount;

    vec3 uv = vec3(texCoord, 0.0);
    float depth = 0.0;
    for(int i = 0; i < layerCount; i++)
    {
        uv -= delta * vec3(s.Depth, s.Depth, 1.0);

        depth = 1.0 - texture(s.HeightTexture, uv.xy * vec2(1, -1)).a;
        if(uv.z > depth) break;
    }

    vec3 previousUV = uv + delta * vec3(s.Depth, s.Depth, 1.0);
    float afterDepth  = depth - uv.z;
    float beforeDepth = 1.0 - texture(s.HeightTexture, previousUV.xy * vec2(1, -1)).a - uv.z + -delta.z;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);

    return mix(uv, previousUV, weight).xy;
}