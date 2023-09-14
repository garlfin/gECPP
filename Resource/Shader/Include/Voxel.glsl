#ifndef VOXEL_TEXTURE_LOCATION
    #define VOXEL_TEXTURE_LOCATION 0
#endif

#ifdef VOXEL_WRITE
    layout(binding = VOXEL_TEXTURE_LOCATION, rgba16f) restrict writeonly image3D VoxelOut;
#endif

#extension GL_ARB_bindless_texture : require

struct VoxelGridData
{
    vec3 Minimum;
    float VoxelScale; // Packs in there nicely, wasted space before.
    vec3 Maximum;
    float MipCount;
    sampler3D Texture;
};

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

void VoxelWrite(vec3 color, float m, float r)
{
    ivec3 gridResolution = textureSize(VoxelGrid.Texture, 0);
}

vec3 VoxelRead(vec3 WorldPos, uint mip)
{
    return vec3(0);
}