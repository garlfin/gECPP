#include "Camera.glsl"
#include "Math.glsl"

// Functions
float InterleavedGradientNoise(vec2);

// Helper Variables
#ifdef FRAGMENT_SHADER
    float IGNSample = InterleavedGradientNoise(gl_FragCoord.xy);
#endif

#ifdef COMPUTE_SHADER
    float IGNSample = InterleavedGradientNoise(gl_GlobalInvocationID.xy);
#endif

// https://www.shadertoy.com/view/fdl3zn
float InterleavedGradientNoise(vec2 uv)
{
    uint frame = Camera.Frame % 128;
    if((frame & 2u) != 0u) uv = vec2(-uv.y, uv.x);
    if((frame & 1u) != 0u) uv.x = -uv.x;

    return fract(uv.x * 0.7548776662 + uv.y * 0.56984029 + float(frame) * 0.41421356);
}