layout(location = 1) uniform sampler2D ColorTexture;

struct VertexOut
{
    vec2 UV;
    vec4 Color;
};

in VertexOut VertexIn;

out vec4 Color;

void main()
{
    Color = VertexIn.Color * texture(ColorTexture, VertexIn.UV);
}