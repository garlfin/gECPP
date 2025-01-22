//
// Created by scion on 1/20/2025.
//

#include "Asset.h"

namespace gE
{
    File::File(const Path& path, const TypeSystem::Type* type) :
        _path(path),
        _uuid(HashPath(_path)),
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

    Bank::Bank(Window* window, const Path& path, AssetLoadMode mode) : _path(path), _window(window)
    {
        _stream.open(_path, std::ios::in | std::ios::binary);

        GE_ASSERTM(_stream.is_open(), "COULD NOT OPEN BANK!");
    }

    const File* Bank::LoadFile(const Path& path)
    {
        const File* file = FindFile(path);

        if(file)
            file->Load();

        return file;
    }

    const File* Bank::FindFile(const UUID& uuid)
    {
        const auto it = _files.find(File(uuid));
        return it == _files.end() ? nullptr : &*it;
    }

    const File* Bank::FindFile(const Path& path)
    {
        return FindFile(HashPath(path));
    }

    void Bank::UnloadFile(const File& file)
    {
        if((Bank*) file.GetBank() != this) return;
        _files.erase(file);
    }

    bool Bank::UnloadFile(const Path& path)
    {
        return UnloadFile(HashPath(path));
    }

    bool Bank::UnloadFile(const UUID& uuid)
    {
        const File* file = FindFile(uuid);

        if(!file) return false;
        UnloadFile(*file);
        return true;
    }

    const File* Bank::AddFile(File&& file)
    {
        if((Bank*) file.GetBank() != this) return nullptr;
        return &*_files.insert(std::move(file)).first; // dumb
    }

    void Bank::RemoveFile(const File& file)
    {
        if((Bank*) file.GetBank() != this) return;
        _files.erase(file);
    }

    UUID HashPath(const Path& path)
    {
        const std::string str = path.string();
        return std::bit_cast<__uint128_t>(CityHash::CityHash128(str.c_str(), str.length()));
    }

    void File::ISerialize(istream& in, SETTINGS_T s)
    {
        _bank = s.Bank;

        if(_bank) _bank->MoveStreamToFile(_uuid);

        _path = Read<std::string>(in);
        _uuid = Read<UUID>(in);
        _type = ReadType<Window*>(in);
        GE_ASSERT(_type);

        if((bool) (s.LoadMode & AssetLoadMode::Files))
            _asset = ref_cast((Asset*) _type->Factory(in, &_bank->GetWindow()));
    }

    void File::IDeserialize(ostream& out) const
    {
        Write(out, _path.string());
        Write(out, _uuid);

        GE_ASSERTM(!_asset.IsFree(), "Asset must be loaded.");
        GE_ASSERT(_type);
        WriteType<Window*>(out, *_type);

        Write(out, *_asset);
    }

    Reference<Asset> File::Load() const
    {
        if(!_bank) return _asset;

        std::istream& in = _bank->MoveStreamToFile(_uuid);

        Read<std::string>(in);
        Read<UUID>(in);
        ReadType<Window*>(in);

        _asset = ref_cast((Asset*) _type->Factory(in, &_bank->GetWindow()));
        _weakAsset = _asset;

        return _asset;
    }

    Reference<Asset> File::LoadWeak() const
    {
        Load();
        return MakeWeak();
    }

    Reference<Asset> File::MakeWeak() const
    {
        Reference<Asset> reference = _asset;
        _asset.Free();
        return reference;
    }

    Reference<Asset> File::Lock() const
    {
        return _asset = _weakAsset;
    }
}
