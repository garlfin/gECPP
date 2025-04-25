layout(binding = 0) uniform sampler2D Color;

in vec2 UV;
out vec4 FragColor;

void main()
{
    FragColor = textureLod(Color, UV, 0.f);
}