#include "Bindless.glsl"

#ifndef VOXEL_TEXTURE_LOCATION
    #define VOXEL_TEXTURE_LOCATION 0
#endif

#ifndef VOXEL_DATA_LOCATION
    #define VOXEL_DATA_LOCATION 1
#endif

uniform layout(binding = VOXEL_TEXTURE_LOCATION, rgba16f) restrict writeonly image3D VoxelColorOut;
uniform layout(binding = VOXEL_DATA_LOCATION, rgba16f) restrict writeonly image3D VoxelDataOut;

struct VoxelGridData
{
    vec3 Minimum;
    float VoxelScale;
    vec3 Maximum;
    float MipCount; // Cleaner than textureQueryLevels, but I might switch it out if I need different data there
    BINDLESS_TEXTURE(sampler3D, Color);
    BINDLESS_TEXTURE(sampler3D, Data);
};

#if defined(FRAGMENT) && !defined(GL_ARB_bindless_texture)
uniform sampler3D VoxelColor;
uniform sampler3D VoxelData;
#endif

#ifndef VOXEL_UNIFORM_LOCATION
    #define VOXEL_UNIFORM_LOCATION 4
#endif

#ifndef VOXEL_UNIFORM_LAYOUT
    #define VOXEL_UNIFORM_LAYOUT std140
#endif

layout(VOXEL_UNIFORM_LAYOUT, binding = VOXEL_UNIFORM_LOCATION) uniform VoxelGridUniform
{
    VoxelGridData VoxelGrid;
};

struct Voxel
{
    vec3 Color;
    float Metallic;
    float Roughness;
    float Emission;
    bool Solid;
};
