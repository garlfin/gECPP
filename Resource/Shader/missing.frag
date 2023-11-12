#include "Include/Camera.glsl"

#define CHECKER_SIZE 10
#define PI 3.14159

out vec4 FragColor;

void main()
{
   float sine = sin(Camera.Time * PI * 2) * 0.5 + 0.5;
   vec2 pos = floor(gl_FragCoord.xy / CHECKER_SIZE);
   float pattern = mod(pos.x + mod(pos.y, 2.0), 2.0);

   vec3 color = vec3(1, 0, 1) * pattern * sine;
   FragColor = vec4(color, 1);
}