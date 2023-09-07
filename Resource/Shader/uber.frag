struct VertexOut
{
    vec3 FragPos;
    vec2 UV;
    mat3 TBN;
};

in VertexOut Vertex;

out vec4 FragColor;

void main()
{
    FragColor = vec4(Vertex.TBN[2], 1);
}