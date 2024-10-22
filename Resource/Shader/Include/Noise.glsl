#include "Camera.glsl"

#ifndef PI
    #define PI 3.141592
#endif

// Functions
float InterleavedGradientNoise(vec2);
float InterleavedGradientNoise(vec2, int);

// Helper Variables
#ifdef FRAGMENT_SHADER
    float IGNSample = InterleavedGradientNoise(gl_FragCoord.xy);
#endif

float InterleavedGradientNoise(vec2 uv)
{
    return InterleavedGradientNoise(uv, TAA_SAMPLE_SQUARED);
}

float InterleavedGradientNoise(vec2 uv, int mod)
{
#ifdef ENABLE_TAA
    uv += 5.588238 * (Camera.Frame % mod);
#endif
    return fract(52.9829189 * fract(dot(uv, vec2(0.06711056, 0.00583715))));
}