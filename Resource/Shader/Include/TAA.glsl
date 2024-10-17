#ifndef TAA_SAMPLE_COUNT
    #define TAA_SAMPLE_COUNT 16
#endif

#define TAA_SAMPLE_SQUARED (TAA_SAMPLE_COUNT * TAA_SAMPLE_COUNT)

const vec2 HaltonSequence[16] =
{
    vec2(0.5, 0.333333),
    vec2(0.25, 0.666667),
    vec2(0.75, 0.111111),
    vec2(0.125, 0.444444),
    vec2(0.625, 0.777778),
    vec2(0.375, 0.222222),
    vec2(0.875, 0.555556),
    vec2(0.0625, 0.888889),
    vec2(0.5625, 0.037037),
    vec2(0.3125, 0.37037),
    vec2(0.8125, 0.703704),
    vec2(0.1875, 0.148148),
    vec2(0.6875, 0.481481),
    vec2(0.4375, 0.814815),
    vec2(0.9375, 0.259259),
    vec2(0.03125, 0.592593)
};

vec2 Jitter(uint i, vec2 size)
{
#ifdef ENABLE_TAA
    vec2 halton = HaltonSequence[i % TAA_SAMPLE_COUNT];
    return (halton * 2.0 - 1.0)  / size;
#else
    return vec2(0.0);
#endif
}

mat4 JitterMat(vec2 offset)
{
    mat4 jitter = mat4(1.0);
    jitter[2].xy = offset;
    return jitter;
}