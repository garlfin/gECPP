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
    vec3 Center;
    float VoxelScale;
    vec3 Scale;
    uint MipCount; // Cleaner than textureQueryLevels, but I might switch it out if I need different data there
    BINDLESS_TEXTURE(sampler3D, Color);
    BINDLESS_TEXTURE(sampler3D, Data);
};

#if defined(FRAGMENT_SHADER) && !defined(GL_ARB_bindless_texture)
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
    vec4 MREA; // Meal Ready to Eat! (Metallic, Roughness, Emission, Alpha)
};

void WriteVoxel(vec3 pos, Voxel voxel)
{
    // Outside the grid

    vec3 boxMin = VoxelGrid.Center - VoxelGrid.Scale;
    vec3 boxMax = VoxelGrid.Center + VoxelGrid.Scale;

    // if(any(lessThan(pos, boxMin)) || any(greaterThan(pos, boxMax))) return;

    vec3 localPos = (pos - boxMin) / (VoxelGrid.Scale * 2.0);
    ivec3 coord = ivec3(round(localPos * imageSize(VoxelColorOut)));

    imageStore(VoxelColorOut, coord, vec4(voxel.Color, 1.0));
    imageStore(VoxelDataOut, coord, voxel.MREA);
}




