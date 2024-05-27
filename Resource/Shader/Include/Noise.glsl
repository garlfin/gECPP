#include "Camera.glsl"

#ifndef PI
    #define PI 3.141592
#endif

// Functions
float InterleavedGradientNoise(vec2);

// Helper Variables
#ifdef FRAGMENT_SHADER
    float IGNSample = InterleavedGradientNoise(gl_FragCoord.xy);
#endif

float InterleavedGradientNoise(vec2 uv)
{
    uv = uv + 5.588238 * (Camera.Frame % TAA_SAMPLE_SQUARED);
    return fract(52.9829189 * fract(dot(uv, vec2(0.06711056, 0.00583715))));
}