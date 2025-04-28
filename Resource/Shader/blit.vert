in vec2 Position;

out vec2 UV;

void main()
{
    UV = Position;
    gl_Position = vec4(UV * 2 - 1 , 0, 1);
}