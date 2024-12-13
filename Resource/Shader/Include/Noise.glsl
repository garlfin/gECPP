#include "Camera.glsl"
#include "Math.glsl"

// Functions
float InterleavedGradientNoise(vec2, int);

// Helper Variables
#ifdef FRAGMENT_SHADER
    float IGNSample = InterleavedGradientNoise(gl_FragCoord.xy, 64);
#endif

float InterleavedGradientNoise(vec2 uv, int mod)
{
#ifdef ENABLE_TAA
    uv += (Camera.Frame % mod) * vec2(32.665, 11.815);
#endif
    return fract(52.9829189 * fract(uv.x * 0.06711056 + uv.y * 0.00583715));
}