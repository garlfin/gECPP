#include "Math.glsl"

// https://github.com/TheRealMJP/SHforHLSL/blob/main/SH.hlsli

#define SH_COSA0 PI
#define SH_COSA1 (2.0 * PI / 3.0)
#define SH_COSA2 (PI / 4.0)

#define SH_BL0 (1.0 / (2.0 * SQRT_PI))
#define SH_BL1 (sqrt(3.0) * SH_BL0)

#define SH_BL2_MN1 (sqrt(15.0) * SH_BL0)
#define SH_BL2_MN2 SH_BL2_MN1
#define SH_BL2_M0 (sqrt(5.0) * SH_BL0 * SH_BL0)
#define SH_BL2_M1 SH_BL2_MN1
#define SH_BL2_M2 (sqrt(15.0) * SH_BL0 * SH_BL0)

// L2 Spherical Harmonics
struct ColorHarmonic
{
    vec3 Coefficients[9];
};

struct Harmonic
{
    float Coefficients[9];
};

// Functions
void SH_Project(inout ColorHarmonic h, vec3 direction, vec3 color);
void SH_NormalizeHarmonic(inout ColorHarmonic h, uint samples);
ColorHarmonic SH_Mix(ColorHarmonic a, ColorHarmonic b, float t);
ColorHarmonic SH_Add(ColorHarmonic a, ColorHarmonic b);
Harmonic SH_CreateCosineLobeHarmonic(vec3 direction);
vec3 SH_GetColor(ColorHarmonic h, vec3 direction);

// Implementation
void SH_Project(inout ColorHarmonic h, vec3 direction, vec3 color)
{
    h.Coefficients[0] += SH_BL0 * color;

    h.Coefficients[1] += SH_BL1 * direction.y * color;
    h.Coefficients[2] += SH_BL1 * direction.z * color;
    h.Coefficients[3] += SH_BL1 * direction.x * color;

    h.Coefficients[4] += SH_BL2_MN2 * direction.x * direction.y * color;
    h.Coefficients[5] += SH_BL2_MN1 * direction.y * direction.z * color;
    h.Coefficients[6] += SH_BL2_M0 * (3.0 * direction.z * direction.z - 1.0) * color;
    h.Coefficients[7] += SH_BL2_M1 * direction.x * direction.z * color;
    h.Coefficients[8] += SH_BL2_M2 * (direction.x * direction.x - direction.y * direction.y) * color;
}

void SH_NormalizeHarmonic(inout ColorHarmonic h, uint samples)
{
    const float weight = 1.0 / samples;

    #pragma unroll
    for(uint i = 0; i < 9; i++)
        h.Coefficients[i] *= weight;
}

ColorHarmonic SH_Mix(ColorHarmonic a, ColorHarmonic b, float t)
{
    ColorHarmonic h;

    #pragma unroll
    for(uint i = 0; i < 9; i++)
        h.Coefficients[i] = mix(a.Coefficients[i], b.Coefficients[i], t);

    return h;
}

ColorHarmonic SH_Add(ColorHarmonic a, ColorHarmonic b)
{
    for(uint i = 0; i < 9; i++)
        a.Coefficients[i] += b.Coefficients[i];

    return a;
}

Harmonic SH_CreateCosineLobeHarmonic(vec3 direction)
{
    Harmonic h;

    h.Coefficients[0] = SH_BL0 * SH_COSA0;

    h.Coefficients[1] = SH_BL1 * direction.y * SH_COSA1;
    h.Coefficients[2] = SH_BL1 * direction.z * SH_COSA1;
    h.Coefficients[3] = SH_BL1 * direction.x * SH_COSA1;

    h.Coefficients[4] = SH_BL2_MN2 * direction.x * direction.y * SH_COSA2;
    h.Coefficients[5] = SH_BL2_MN1 * direction.y * direction.z * SH_COSA2;
    h.Coefficients[6] = SH_BL2_M0 * (3.0 * direction.z * direction.z - 1.0) * SH_COSA2;
    h.Coefficients[7] = SH_BL2_M1 * direction.x * direction.z * SH_COSA2;
    h.Coefficients[8] = SH_BL2_M2 * (direction.x * direction.x - direction.y * direction.y) * SH_COSA2;

    return h;
}

vec3 SH_GetColor(ColorHarmonic h, vec3 direction)
{
    Harmonic cosine = SH_CreateCosineLobeHarmonic(direction);

    vec3 color = vec3(0.0);

    #pragma unroll
    for(int i = 0; i < 9; i++)
        color += cosine.Coefficients[i] * h.Coefficients[i];

    return color;
}