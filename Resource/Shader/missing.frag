#include "Include/Camera.glsl"
#include "Include/Scene.glsl"
#include "Include/Voxel.glsl"

#define CHECKER_SIZE 10
#define PI 3.14159

struct VertexOut
{
   vec3 FragPos;
   vec2 UV;
   vec4 PreviousUV;
   vec4 CurrentUV;
   vec4 FragPosLightSpace[MAX_LIGHTS];
   mat3 TBN;
};

in VertexOut VertexIn;

layout(location = 0) out vec4 FragColor;

void main()
{
   if(!bool(Scene.State & ENABLE_COLOR)) return;

   if(!bool(Scene.State & ENABLE_VOXEL_WRITE))
   {
      float sine = sin(Camera.Frame * PI / 60.0) * 0.5 + 0.5;
      vec2 pos = floor(gl_FragCoord.xy / CHECKER_SIZE);
      float pattern = mod(pos.x + mod(pos.y, 2.0), 2.0);

      vec3 color = vec3(1, 0, 1) * pattern * sine;
      FragColor = vec4(color, 1);
   }
   else
   {
      FragColor = vec4(1, 0, 1, 1);

      ivec3 texel = Voxel_WorldToTexel(VertexIn.FragPos, imageSize(VoxelColorOut).x);
      imageStore(VoxelColorOut, texel, PackColor(FragColor));
   }
}