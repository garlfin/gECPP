#include "Camera.glsl"
#include "Voxel.glsl"
#include "Shadow.glsl"

// Functions
#ifdef EXT_BINDLESS
RayResult SS_Trace(Ray);
RayResult SS_TraceOffset(Ray, vec3);
#endif

// Helper Functions
vec3 SS_WorldToUV(vec3);
vec3 TexelToUV(vec3, uint); // CellCount
ivec3 UVToTexel(vec3, uint); // CellCount
ivec3 WorldToTexel(vec3, uint); // CellCount
vec3 SS_WorldToAlignedUV(vec3, uint); // CellCount
vec3 AlignWorldToCell(vec3, uint); // CellCount
float SS_CrossCell(inout vec3, vec3, uint); // CellCount
