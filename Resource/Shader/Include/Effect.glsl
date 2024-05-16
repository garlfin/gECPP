#include "Bindless.glsl"

struct ParallaxEffectSettings
{
    float Depth;
    int MinLayers;
    int MaxLayers;
};

vec2 ParallaxMapping(vec3 viewDir, sampler2D, Vertex, ParallaxEffectSettings s);

// Implementation
vec2 ParallaxMapping(vec3 viewDir, sampler2D tex, Vertex vert, ParallaxEffectSettings s)
{
    s.Depth *= 0.1;

    viewDir = transpose(vert.TBN) * viewDir;

    float NdotV = 1.0 - abs(dot(vec3(0.0, 0.0, 1.0), viewDir));
    int layerCount = int(mix(s.MinLayers, s.MaxLayers, NdotV));

    vec3 delta = vec3(viewDir.xy / viewDir.z, -1.0) / layerCount;

    vec3 uv = vec3(vert.UV, 0.0);
    float depth = 0.0;
    for(int i = 0; i < layerCount; i++)
    {
        uv -= delta * vec3(s.Depth, s.Depth, 1.0);

        depth = 1.0 - texture(tex, uv.xy * vec2(1, -1)).a;
        if(uv.z > depth) break;
    }

    vec3 previousUV = uv + delta * vec3(s.Depth, s.Depth, 1.0);
    float afterDepth = depth - uv.z;
    float beforeDepth = 1.0 - texture(tex, previousUV.xy * vec2(1, -1)).a - uv.z - delta.z;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);

    return mix(uv, previousUV, weight).xy;
}