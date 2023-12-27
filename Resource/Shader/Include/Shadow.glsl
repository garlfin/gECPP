#include "Camera.glsl"
#include "Bindless.glsl"
#include "Vertex.glsl"
#include "TAA.glsl"

// In percent
#ifndef DIRECTIONAL_SHADOW_BIAS
    #define DIRECTIONAL_SHADOW_BIAS 0.01
#endif

#ifndef DIRECTIONAL_SHADOW_SAMPLES
    #define DIRECTIONAL_SHADOW_SAMPLES 16
#endif

#define SOFT_SHADOW_AVERAGE

#define SOFT_SHADOW (defined(SOFT_SHADOW_AVERAGE) || defined(SOFT_SHADOW_MIN))

#if SOFT_SHADOW
    #ifndef DIRECTIONAL_SHADOW_BLOCKER_SAMPLES
        #define DIRECTIONAL_SHADOW_BLOCKER_SAMPLES 8
    #endif

    #ifndef DIRECTIONAL_SHADOW_BLOCKER_RADIUS
        #define DIRECTIONAL_SHADOW_BLOCKER_RADIUS 0.1
    #endif
#endif

#ifndef DIRECTIONAL_SHADOW_RADIUS
    #define DIRECTIONAL_SHADOW_RADIUS 0.05
#endif

#ifndef DIRECTIONAL_SHADOW_MIN_RADIUS
    #define DIRECTIONAL_SHADOW_MIN_RADIUS 0.02
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
vec3 NDCLight(vec4, vec2);

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
	vec3 fragPos = NDCLight(frag.PositionLightSpace, light.Planes);

    float blocker = 0.0;
#ifdef SOFT_SHADOW_AVERAGE
    float blockerCount = 0.0;
#endif
    float shadow = 0.0;

#if SOFT_SHADOW
    for(uint i = 0; i < DIRECTIONAL_SHADOW_BLOCKER_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i, DIRECTIONAL_SHADOW_BLOCKER_SAMPLES, IGNSample * PI * 2.0);
        vec3 uv = fragPos;
        uv.xy += offset * DIRECTIONAL_SHADOW_BLOCKER_RADIUS / light.PackedSettings;

    #ifdef GL_ARB_bindless_texture
        float z = texture(sampler2D(light.Depth), uv.xy).r;
    #else
        float z = 1.0;
    #endif
        z = LinearizeDepthOrtho(z, light.Planes);
        z = uv.z - z;

        float shadowSample = z > bias ? 1.0 : 0.0;

        #ifdef SOFT_SHADOW_AVERAGE
            blocker += z * shadowSample;
            blockerCount += shadowSample;
        #else
            blocker = max(blocker, z * shadowSample);
        #endif
    }

    #ifdef SOFT_SHADOW_AVERAGE
        blocker = max(blocker / blockerCount * DIRECTIONAL_SHADOW_RADIUS, DIRECTIONAL_SHADOW_MIN_RADIUS);
    #else
        blocker = max(blocker, DIRECTIONAL_SHADOW_MIN_RADIUS);
    #endif
#else
    blocker = DIRECTIONAL_SHADOW_RADIUS;
#endif

    for(uint i = 0; i < DIRECTIONAL_SHADOW_SAMPLES; i++)
    {
        vec2 offset = VogelDisk(i, DIRECTIONAL_SHADOW_SAMPLES, IGNSample * PI * 2.0);
        vec3 uv = fragPos;
        uv.xy += offset * blocker / light.PackedSettings;

    #ifdef GL_ARB_bindless_texture
        float z = texture(sampler2D(light.Depth), uv.xy).r;
    #else
        float z = 1.0;
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
    uv = uv + 5.588238 * (Camera.Frame % TAA_SAMPLE_SQUARED);
    return fract(52.9829189 * fract(dot(uv, vec2(0.06711056, 0.00583715))));
}

vec2 VogelDisk(uint i, uint count, float phi)
{
    float radius = sqrt((i + 0.5) / count);
    float theta = i * GOLDEN_ANGLE + phi;
    return vec2(cos(theta), sin(theta)) * radius;
}

vec3 NDCLight(vec4 frag, vec2 planes)
{
	frag.xyz = frag.xyz / frag.w;
	frag.xyz = frag.xyz * 0.5 + 0.5;
	frag.z = LinearizeDepthOrtho(frag.z, planes);
	return frag.xyz;
}