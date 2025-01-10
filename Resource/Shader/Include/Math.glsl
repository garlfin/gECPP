#ifndef EPSILON
    #define EPSILON 0.001
#endif

#define PI 3.141592
#define SQRT_PI 1.772454
#define GOLDEN_ANGLE 2.4
#define FLT_INF uintBitsToFloat(0x7F800000)

// Main Functions
mat3 GetTBN(vec3);
float LinearizeDepthOrtho(float, vec2);
float LinearizeDepthOrthoNDC(float, vec2);
float LinearizeDepth(float, vec2);
float LinearizeDepthNDC(float z, vec2 planes);
float LogarithmizeDepth(float z, vec2 planes);
float LogarithmizeDepthNDC(float z, vec2 planes);
bool TexcoordOutOfBounds(vec2 uv);
vec2 VogelDisk(uint i, uint count, float phi);
float saturate(float a) { return clamp(a, 0.0, 1.0); }
vec2 saturate(vec2 a) { return clamp(a, vec2(0.0), vec2(1.0)); }
vec3 saturate(vec3 a) { return clamp(a, vec3(0.0), vec3(1.0)); }
vec4 saturate(vec4 a) { return clamp(a, vec4(0.0), vec4(1.0)); }
float Sign(float f) { return f < 0 ? -1 : 1; }
vec2 Sign(vec2 v) { return vec2(Sign(v.x), Sign(v.y)); }
vec3 Sign(vec3 v) { return vec3(Sign(v.x), Sign(v.y), Sign(v.z)); }
float VDCInverse(uint bits);
vec2 Hammersley(uint i, uint sampleCount);
float CreateBias(float);
vec3 ToHemisphere(vec2 vec);
vec3 ToSphere(vec2 vec);

#define RAY_RESULT_NO_HIT 0
#define RAY_RESULT_HIT 1
#define RAY_RESULT_TOO_FAR 2
#define RAY_RESULT_EXHAUSTED 3
#define RAY_RESULT_OUT_OF_BOUNDS 4
#define RAY_RESULT_PRECISION_ERROR 5

struct Ray
{
    vec3 Position;
    float Length;
    vec3 Direction;
    int BaseMip;
};

struct RayResult
{
    vec3 Position;
    float Distance;
    vec3 Normal;
    int Result;
};

const RayResult DefaultRayResult = { vec3(0.0), 0.0, vec3(0.0), RAY_RESULT_NO_HIT };

// Implementation
mat3 GetTBN(vec3 normal)
{
    vec3 up = abs(normal.z) < (1.0 - EPSILON) ? vec3(0.0, 0.0, 1.0) : vec3(1, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);

    return mat3(tangent, bitangent, normal);
}

float LinearizeDepthOrtho(float z, vec2 planes)
{
    return LinearizeDepthOrthoNDC(z * 2.0 - 1.0, planes);
}

float LinearizeDepthOrthoNDC(float z, vec2 planes)
{
    return planes.x + z * (planes.y - planes.x);
}

float LinearizeDepthNDC(float z, vec2 planes)
{
    return 2.0 * planes.x * planes.y / (planes.y + planes.x - z * (planes.y - planes.x));
}

float LinearizeDepth(float z, vec2 planes)
{
    return LinearizeDepthNDC(z * 2.0 - 1.0, planes);
}

float LogarithmizeDepthNDC(float z, vec2 planes)
{
    return (1.0 / z - 1.0 / planes.x) / (1.0 / planes.y - 1.0 / planes.x);
}

float LogarithmizeDepth(float z, vec2 planes)
{
    return LogarithmizeDepthNDC(z * 2.0 - 1.0, planes);
}

bool TexcoordOutOfBounds(vec2 uv)
{
    return any(lessThan(uv, vec2(0.0))) || any(greaterThan(uv, vec2(1.0)));
}

vec2 VogelDisk(uint i, uint count, float phi)
{
    float radius = sqrt((i + 0.5) / count);
    float theta = i * GOLDEN_ANGLE + phi;
    return vec2(cos(theta), sin(theta)) * radius;
}

// How someone came up with this, I don't know.
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
float VDCInverse(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint sampleCount)
{
    return vec2(float(i) / float(sampleCount), VDCInverse(i));
}

vec2 Rotate(vec2 vec, float angle)
{
    vec2 result;

    float cosTheta = cos(angle);
    float sinTheta = sin(angle);

    result.x = vec.x * cosTheta - vec.y * sinTheta;
    result.y = vec.x * sinTheta + vec.y * cosTheta;

    return result;
}

vec3 ToHemisphere(vec2 vec)
{
    float phi = vec.y * 2.0 * PI;
    float cosTheta = 1.0 - vec.x;
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    return vec3(cos(phi) * cosTheta, sin(phi) * sinTheta, cosTheta);
}

vec3 ToSphere(vec2 vec)
{
    vec = vec * 2.0 - 1.0;
    vec *= PI;

    float cosTheta = cos(vec.x);

    return vec3(cos(vec.y) * cosTheta, sin(vec.y) * sin(vec.x), cosTheta);
}

float CreateBias(float bias)
{
    if(bias < 0.0)
        return 1.0 / -bias;
    return 1.0 + bias;
}