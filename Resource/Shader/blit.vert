const vec2 Vertices[6] =
{
        vec2(0, 0),
        vec2(0, 1),
        vec2(1, 0),

        vec2(1, 0),
        vec2(0, 1),
        vec2(1, 1)
};

out vec2 UV;

void main()
{
    UV = Vertices[gl_VertexID];
    gl_Position = vec4(UV * 2 - 1 , 0, 1);

}