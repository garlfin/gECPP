#include "Bindless.glsl"
#include "Vertex.glsl"

float GetShadowDirectional(inout Fragment frag, Light light)
{
#ifdef EXT_BINDLESS
    float shadow = lightCoord.z - texture(sampler2D(light.Depth), lightCoord.xy).r;
#else
    // TODO: THIS!!!
    float shadow = 0;
#endif
    shadow = shadow > bias ? 0 : 1;
    shadow = TexcoordOutOfBounds(lightCoord.xy) ? 1.0 : shadow;
}