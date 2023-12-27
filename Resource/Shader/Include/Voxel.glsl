#include "Bindless.glsl"

#ifndef VOXEL_TEXTURE_LOCATION
    #define VOXEL_TEXTURE_LOCATION 0
#endif

#ifndef VOXEL_DATA_LOCATION
    #define VOXEL_DATA_LOCATION 1
#endif

uniform layout(binding = VOXEL_TEXTURE_LOCATION, rgba16f) restrict writeonly image3D VoxelColorOut;
uniform layout(binding = VOXEL_DATA_LOCATION, r8ui) restrict writeonly uimage3D VoxelDataOut;

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
    vec4 RSMA; // (Metallic, Roughness, Specular, Alpha)
};

void WriteVoxel(vec3 pos, Voxel voxel)
{
    // Outside the grid
    // if(any(lessThan(pos, boxMin)) || any(greaterThan(pos, boxMax))) return;

    vec3 localPos = (pos - VoxelGrid.Center) / VoxelGrid.Scale;
    localPos = localPos * 0.5 + 0.5;

    ivec3 coord = ivec3(floor(localPos * imageSize(VoxelColorOut)));

    voxel.RSMA = clamp(voxel.RSMA, vec4(0.0), vec4(1.0));

    uint packedRSMA = 0; // 3, 3, 1, 1
    packedRSMA |= uint(voxel.RSMA.r * 32.0); // Roughness
    packedRSMA |= uint(voxel.RSMA.g * 32.0) << 3; // Specular
    packedRSMA |= uint(voxel.RSMA.b) << 6; // Metallic
    packedRSMA |= uint(voxel.RSMA.a) << 7; // Alpha (Solid)

    imageStore(VoxelColorOut, coord, vec4(voxel.Color, 1.0));
    imageStore(VoxelDataOut, coord, ivec4(packedRSMA));
}

// Voxel ReadVoxel(vec3);
// Voxel ReadVoxel(ivec3);