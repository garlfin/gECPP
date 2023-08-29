layout(location = 0) in vec3 Position;
layout(location = 0) in vec2 UV;
layout(location = 0) in vec3 Normal;
layout(location = 0) in vec3 Tangent;

#define MAX_OBJECT 64

struct Camera
{
    vec2 ClipPlanes;
    float FOV;

    mat4 View;
    mat4 Projection;
};

struct Object
{
    uint InstanceCount;
    mat4 Model[MAX_OBJECT];
    mat3 Normal[MAX_OBJECT];
};

struct VertexOut
{
    vec3 FragPos;
    vec2 UV;
    mat3 TBN;
};

out VertexOut Out;

void main()
{
    Out.FragPos = Position;
    Out.UV = UV;
    Out.TBN = mat3(Tangent, normalize(cross(Tangent, Normal)), Normal);
}