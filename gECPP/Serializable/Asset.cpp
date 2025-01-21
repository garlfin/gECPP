//
// Created by scion on 1/20/2025.
//

#include "Asset.h"

gE::File::File(const Path& path, const TypeSystem::Type* type) :
    _path(path),
    _type(type)
{

}

gE::File::File(const Path& path, const Reference<Asset>& asset) :
    _path(path),
    _type(ValidateAssetType(asset)),
    _asset(asset)
{
}

const TypeSystem::Type* gE::File::ValidateAssetType(const Asset& asset)
{
    const TypeSystem::Type* type = asset.GetType();

    GE_ASSERT(type);
    return type;
}

void gE::File::ISerialize(istream& in, SETTINGS_T s)
{
    _path = Read<std::string>(in);
    _type = ReadType<Window*>(in);
    GE_ASSERT(_type);
}

void gE::File::IDeserialize(ostream& out) const
{
}

gE::Reference<gE::Asset> gE::File::Load()
{
}
