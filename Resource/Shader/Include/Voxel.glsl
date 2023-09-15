#ifndef VOXEL_TEXTURE_LOCATION
    #define VOXEL_TEXTURE_LOCATION 0
#endif

#ifdef WRITE_VOXEL
    uniform layout(binding = VOXEL_TEXTURE_LOCATION, rgba16f) restrict writeonly image3D VoxelOut;
#endif

#extension GL_ARB_bindless_texture : require

struct Voxel
{
    vec3 Color;
    float Metallic; // 8 bits
    float Roughness; // 8 bits
    bool Solid; // 1 bit
};

vec4 PackVoxel(const Voxel voxel)
{
    uint data = 0;

    data |= (uint(floor(voxel.Metallic * 255)) & 0xFF);
    data |= (uint(floor(voxel.Roughness * 255)) & 0xFF) << 8;
    data |= (uint(voxel.Solid) & 0x1) << 16;

    return vec4(voxel.Color, uintBitsToFloat(data));
}

Voxel UnpackVoxel(const vec4 col)
{
    uint data = floatBitsToUint(col.a);
    float m = (data & 0xFF) / 255.f;
    float r = (data >> 8 & 0xFF) / 255.f;
    bool solid = bool(data >> 16 & 0x1);
    return Voxel(col.rgb, m, r, solid);
}

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

vec3 WorldToLocalVoxelTexcoord(vec3 worldPos)
{
    return (worldPos - VoxelGrid.Minimum) / textureSize(VoxelGrid.Texture, 0);
}

ivec3 WorldToLocalVoxel(vec3 worldPos, out bool outOfBounds)
{
    const vec3 localPos = worldPos - VoxelGrid.Minimum;
    outOfBounds = any(lessThan(localPos, vec3(0))) || any(greaterThan(localPos, VoxelGrid.Maximum));
    return ivec3(floor(mod(localPos, VoxelGrid.Maximum) / VoxelGrid.VoxelScale));
}

ivec3 WorldToLocalVoxel(vec3 worldPos, uint mip)
{
    const vec3 localPos = worldPos - VoxelGrid.Minimum;
    return ivec3(floor(mod(localPos, VoxelGrid.Maximum) / VoxelGrid.VoxelScale)) >> mip;
}

ivec3 WorldToLocalVoxel(vec3 worldPos)
{
    return WorldToLocalVoxel(worldPos, 0u);
}

#ifdef WRITE_VOXEL
void WriteVoxel(vec3 pos, const Voxel voxel, uint mip)
{
    const ivec3 localPos = WorldToLocalVoxel(pos, mip);
    imageStore(VoxelOut, localPos, PackVoxel(voxel));
}

void WriteVoxel(vec3 pos, const Voxel voxel)
{
    WriteVoxel(pos, voxel, 0u);
}
#endif

Voxel ReadVoxel(vec3 worldPos, uint mip)
{
    vec3 texPos = WorldToLocalVoxelTexcoord(worldPos);
    vec3 floorTexPos = vec3(WorldToLocalVoxel(worldPos) / textureSize(VoxelGrid.Texture, 0));
    Voxel voxel = UnpackVoxel(texture(VoxelGrid.Texture, texPos));
    Voxel voxelFlat = UnpackVoxel(texture(VoxelGrid.Texture, floorTexPos));
    voxel.Roughness = voxelFlat.Roughness;
    voxel.Metallic = voxelFlat.Metallic;
    voxel.Solid = voxelFlat.Solid;
    return voxel;
}