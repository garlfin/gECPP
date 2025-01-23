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

    Bank::Bank(Window* window, const Path& path, AssetLoadMode mode) :
        _path(path),
        _stream(_path, std::ios::in | std::ios::binary),
        _window(window)
    {
        GE_ASSERTM(_stream.is_open(), "COULD NOT OPEN BANK!");
    }

    const File* Bank::FindFile(const UUID& uuid)
    {
        const auto it = _files.find(uuid);
        return it == _files.end() ? nullptr : &*it;
    }

    const File* Bank::AddFile(File&& file)
    {
        if(file._bank) return nullptr;

        file._bank = this;
        return &*_files.insert(std::move(file)).first; // dumb
    }

    void Bank::ISerialize(istream& in, SETTINGS_T s)
    {
        // TODO
    }

    void Bank::IDeserialize(ostream& out) const
    {
        // TODO
    }

    void Bank::RemoveFile(const File& file)
    {
        if(file.GetBank() != this) return;
        _files.erase(file);
    }

    std::istream& Bank::MoveStreamToFile(const UUID& uuid)
    {
        return _stream;
    }

    Bank* AssetManager::LoadBank(const Path& path, AssetLoadMode mode)
    {
        const auto& it = _banks.emplace(std::move(ptr_create<Bank>(_window, path, mode)));
        return it.first->GetPointer();
    }

    void AssetManager::LoadBanksInFolder(const Path& folder, AssetLoadMode mode)
    {
        const std::filesystem::directory_iterator directory(folder);
        for(const auto& entry : directory)
            if(const Path& path = entry.path(); path.extension() == ".bnk")
                LoadBank(path, mode);
    }

    Bank* AssetManager::FindBank(const UUID& uuid)
    {
        const auto it = _banks.find(uuid);
        return it == _banks.end() ? nullptr : it->GetPointer();
    }

    const File* AssetManager::LoadFile(const Path& path, AssetLoadMode mode)
    {
        FileLoadArgs args(_window, nullptr, mode);

        std::ifstream stream(path, std::ios::in | std::ios::binary);

        if(!stream.is_open())
        {
            GE_FAIL("COULD NOT OPEN FILE!");
            return nullptr;
        }

        const auto& it = _files.emplace(stream, args);
        return &*it.first;
    }

    const File* AssetManager::AddFile(File&& file)
    {
        if(file.GetBank()) return nullptr;
        return &*_files.insert(std::move(file)).first;
    }

    const File* AssetManager::FindFile(const UUID& uuid)
    {
        const auto& it = _files.find(uuid);
        if(it == _files.end()) return nullptr;
        return &*it;
    }

    bool AssetManager::RemoveBank(const UUID& uuid)
    {
        const auto& it = _banks.find(uuid);

        if(it == _banks.end()) return false;

        _banks.erase(it);
        return true;
    }

    bool AssetManager::RemoveFile(const UUID& uuid)
    {
        const auto& it = _files.find(uuid);

        if(it == _files.end()) return false;

        _files.erase(it);
        return true;
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
            _asset = ref_cast((Asset*) _type->Factory(in, s.Window));
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
        if(!_bank || _asset) return _asset;

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