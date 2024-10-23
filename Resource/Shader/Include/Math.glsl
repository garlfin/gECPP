#ifndef EPSILON
    #define EPSILON 0.01
#endif

#ifndef PI
    #define PI 3.141592
#endif

#ifndef GOLDEN_ANGLE
    #define GOLDEN_ANGLE 2.4
#endif

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