//
// Created by scion on 1/20/2025.
//

#include "Asset.h"

namespace gE
{
    File::File(const Path& path, const TypeSystem::Type* type) :
       _path(path),
       _type(type)
    {

    }

    File::File(const Path& path, const Reference<Asset>& asset) :
        _path(path),
        _type(ValidateAssetType(asset)),
        _weakAsset(asset),
        _asset(asset)
    {
    }

    File::File(const Path& path, const WeakReference<Asset>& asset) :
        _path(path),
        _type(ValidateAssetType(asset)),
        _weakAsset(asset)
    {
    }

    const TypeSystem::Type* File::ValidateAssetType(const Asset& asset)
    {
        const TypeSystem::Type* type = asset.GetType();

        GE_ASSERT(type);
        return type;
    }

    Bank::Bank(Window* window, const Path& path) : _path(path), _window(window)
    {
        _stream.open(_path, std::ios::in | std::ios::binary);

        GE_ASSERTM(_stream.is_open(), "COULD NOT OPEN BANK!");
    }

    void File::ISerialize(istream& in, SETTINGS_T s)
    {
        _bank = s;
        _path = Read<std::string>(in);
        _type = ReadType<Window*>(in);
        GE_ASSERT(_type);

        _asset = ref_cast((Asset*) _type->Factory(in, &_bank->GetWindow()));
    }

    void File::IDeserialize(ostream& out) const
    {
        Write(out, _path.string());

        GE_ASSERT(_type);
        WriteType<Window*>(out, *_type);

        Write(out, *_asset);
    }

    Reference<Asset> File::Load()
    {
    }

    Reference<Asset> File::LoadWeak()
    {
        Load();
        return MakeWeak();
    }

    Reference<Asset> File::MakeWeak()
    {
        Reference<Asset> reference = _asset;
        _asset.Free();
        return reference;
    }

    bool File::Lock()
    {
        _asset = _weakAsset;
        return _weakAsset.IsValid();
    }
}
