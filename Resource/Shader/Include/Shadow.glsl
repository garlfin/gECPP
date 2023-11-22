#include "Camera.glsl"
#include "Bindless.glsl"
#include "Vertex.glsl"

// In percents
#ifndef DIRECTIONAL_SHADOW_BIAS
#define DIRECTIONAL_SHADOW_BIAS 0.01
#endif

#ifndef DIRECTIONAL_SHADOW_SAMPLES
#define DIRECTIONAL_SHADOW_SAMPLES 64
#endif

#ifndef PI
#define PI 3.141592
#endif

#define GOLDEN_ANGLE 2.4

// Main Functions
#ifdef FRAGMENT_SHADER
float GetShadowDirectional(const Vertex frag, const Light light);
#endif

// Helper Functions
float LinearizeDepthOrtho(float, vec2);
float LinearizeDepth(float, vec2);
bool TexcoordOutOfBounds(vec2 uv);
float InterleavedGradientNoise(vec2 uv);
vec2 VogelDisk(uint i, uint count, float phi);

// Helper Variables
#ifdef FRAGMENT_SHADER
float IGNSample = InterleavedGradientNoise(gl_FragCoord.xy);
#endif

// Implementation
// Main Functions
#ifdef FRAGMENT_SHADER
float GetShadowDirectional(const Vertex frag, const Light light)
{
    // Linearizing everything now for PCSS later.
    float nDotL = max(dot(frag.Normal, light.Position), 0.0);
    float bias = mix(0.1, 1.0, nDotL) * DIRECTIONAL_SHADOW_BIAS;
    float shadow = 0.0;

    for(uint i = 0; i < DIRECTIONAL_SHADOW_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i, DIRECTIONAL_SHADOW_SAMPLES, IGNSample * PI);
        vec3 uv = frag.PositionLightSpace;
        uv.xy += VogelDisk(i, DIRECTIONAL_SHADOW_SAMPLES, IGNSample * PI) * 0.01 / light.PackedSettings;

    #ifdef EXT_BINDLESS
        float z = texture(sampler2D(light.Depth), uv.xy).r;
    #else
        float z = 0;
    #endif
        z = LinearizeDepthOrtho(z, light.Planes);
        z = uv.z - z;

        float shadowSample = z > bias ? 0.0 : 1.0;
        shadow += TexcoordOutOfBounds(uv.xy) ? 1.0 : shadowSample;
    }

    return shadow / DIRECTIONAL_SHADOW_SAMPLES;
}
#endif

// Helper Functions
float LinearizeDepthOrtho(float z, vec2 planes)
{
    return planes.x + z * (planes.y - planes.x);
}

float LinearizeDepth(float z, vec2 planes)
{
    z = z * 2.0 - 1.0;
    return 2.0 * planes.x * planes.y / (planes.y + planes.x - z * (planes.y - planes.x));
}

bool TexcoordOutOfBounds(vec2 uv)
{
    return any(lessThan(uv, vec2(0.0))) || any(greaterThan(uv, vec2(1.0)));
}

float InterleavedGradientNoise(vec2 uv)
{
    uv = uv + 5.588238 * Camera.Frame;
    return fract(52.9829189 * fract(dot(uv, vec2(0.06711056, 0.00583715))));
}

vec2 VogelDisk(uint i, uint count, float phi)
{
    float radius = sqrt((i + 0.5) / count);
    float theta = i * GOLDEN_ANGLE + phi;
    return vec2(cos(theta), sin(theta)) * radius;
}