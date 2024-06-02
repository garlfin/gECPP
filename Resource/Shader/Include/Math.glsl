#ifndef EPSILON
    #define EPSILON 0.01
#endif

// Main Functions
mat3 GetTangent(vec3);

// Implementation
mat3 GetTangent(vec3 normal)
{
    vec3 up = abs(normal.z) < (1.0 - EPSILON) ? vec3(0.0, 0.0, 1.0) : vec3(1, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);

    return mat3(tangent, bitangent, normal);
}