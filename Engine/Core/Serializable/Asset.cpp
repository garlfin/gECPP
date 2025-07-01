//
// Created by scion on 1/20/2025.
//

#include "Asset.h"

#include <random>
#include <Window.h>
#include <CityHash/city.h>

namespace gE
{
    File::File(Window* window, const Path& path, const Type<Window*>* type) : Serializable(0),
        _window(window),
        _type(type),
        _path(FixPath(path, *_type))
    {
        _UUID = HashPath(_path);
    }

    File::File(Window* window, const Path& path, const Reference<Asset>& asset) : Serializable(0),
        _window(window),
        _type(ValidateAssetType(asset)),
        _path(FixPath(path, *_type)),
        _weakAsset(asset),
        _asset(asset)
    {
        _UUID = HashPath(_path);
        if(_asset)
            _asset->_file = this;
    }

    File::File(Window* window, const Path& path, const WeakReference<Asset>& asset) : Serializable(0),
        _window(window),
        _type(ValidateAssetType(asset)),
        _path(FixPath(path, *_type)),
        _weakAsset(asset)
    {
        _UUID = HashPath(_path);
        if(_weakAsset)
            _weakAsset->_file = this;
    }

    OPERATOR_MOVE_IMPL(File::, File, this->~File(),
        _UUID = o._UUID;
        _window = o._window;
        _type = o._type;
        _path = std::move(o._path);
        _bank = o._bank;
        _weakAsset = std::move(o._weakAsset);
        _asset = std::move(o._asset);

        if(_weakAsset)
            _weakAsset->_file = this;
    );

    const Type<Window*>* File::ValidateAssetType(const Asset& asset)
    {
        const Type<Window*>* type = asset.GetType();

        GE_ASSERT(type);
        return type;
    }

    REFLECTABLE_ONGUI_IMPL(File, );
    REFLECTABLE_NAME_IMPL(File, return _path.string());
    REFLECTABLE_FACTORY_IMPL(File);

    void FileInfo::IDeserialize(istream& in, SETTINGS_T s) {}
    void FileInfo::ISerialize(ostream& out) const {}

    REFLECTABLE_ONGUI_IMPL(FileInfo, );
    REFLECTABLE_FACTORY_IMPL(FileInfo);

    Bank::Bank(Window* window, const Path& path, AssetLoadMode mode) :
        _window(window),
        _path(path),
        _stream(ref_create<std::ifstream>(_path, std::ios::in | std::ios::binary))
    {
        GE_ASSERTM(_stream->fail(), "COULD NOT OPEN BANK!");
    }

    const File* Bank::FindFile(const UUID& uuid) const
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

    void Bank::IDeserialize(istream& in, SETTINGS_T s)
    {
        _path = s.Path;
        _uuid = HashPath(_path);
        _stream = s.Stream;

        if(s.Mode == AssetLoadMode::None)
            return;

        const size_t fileCount = Read<size_t>(in);

        if((bool)(s.Mode & AssetLoadMode::Paths))
            for(size_t i = 0; i < fileCount; i++)
                _fileInfo.insert(ReadSerializable<FileInfo>(in, nullptr));

        auto infoIt = _fileInfo.begin();
        GE_ASSERTM(infoIt != _fileInfo.end() && _fileInfo.size() == fileCount, "PATHS NOT LOADED");

        if((bool)(s.Mode & AssetLoadMode::Files))
            for(size_t i = 0; i < fileCount; i++, ++infoIt)
            {
                FileLoadArgs args{ _window, this, infoIt->Path, s.Mode };
                _files.emplace(in, args);
            }
    }

    void Bank::ISerialize(ostream& out) const
    {
        Write(out, _files.size());

        const size_t infoTableOffset = out.tellp();
        out.seekp(infoTableOffset + sizeof(FileInfo) * _files.size());

        FILE_INFO_SET_T fileInfoSet;
        for(const File& file : _files)
        {
            fileInfoSet.emplace(file._path, file.GetUUID(), out.tellp());
            file.Serialize(out);
        }

        out.seekp(infoTableOffset);
        for(const FileInfo& info : fileInfoSet)
            Write(out, info);
    }

    void Bank::RemoveFile(const File& file)
    {
        if(file.GetBank() != this) return;
        _files.erase(file);
    }

    std::istream& Bank::MoveStreamToFile(const UUID& uuid)
    {
        const auto& it = _fileInfo.find(uuid);
        GE_ASSERT(it != _fileInfo.end());
        return _stream->seekg(it->Offset);
    }

    REFLECTABLE_ONGUI_IMPL(Bank, );
    REFLECTABLE_NAME_IMPL(Bank, return _path.string());
    REFLECTABLE_FACTORY_IMPL(Bank);

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
        FileLoadArgs args(_window, nullptr, path, mode);

        std::ifstream stream(path, std::ios::in | std::ios::binary);

        if(!stream.is_open())
        {
            Log::FatalError(std::format("Could not open \"{}\"", path.string()));
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

    const File* AssetManager::FindFile(const UUID& uuid) const
    {
        if(const auto& it = _files.find(uuid); it != _files.end())
            return &*it;

        for(const auto& bank : _banks)
            if(const File* file = bank->FindFile(uuid))
                return file;

        return nullptr;
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
        const std::string str = FixPath(path).string();
        return std::bit_cast<UUID>(CityHash::CityHash128(str.c_str(), str.length()));
    }

    UUID GenerateUUID()
    {
        static std::random_device randomDevice;
        static std::mt19937_64 twister(randomDevice());
        static std::uniform_int_distribution<u64> random(0, UINT64_MAX);

        UUID uuid = random(twister);
        uuid |= (UUID) random(twister) << 64;

        return uuid;
    }

    Path FixPath(const Path& path)
    {
        Path::string_type str = path;
        std::ranges::replace(str, '\\', '/');
        return Path(std::move(str), std::filesystem::path::generic_format);
    }

    Path FixPath(const Path& path, const Type<Window*>& type)
    {
        Path newPath = FixPath(path);

        if(!type.Extension.empty())
            newPath.replace_extension(type.Extension);

        return newPath;
    }

    void File::IDeserialize(istream& in, SETTINGS_T s)
    {
        _bank = s.Bank;
        _path = s.Path;

        if(_bank) _bank->MoveStreamToFile(GetUUID());

        _type = ReadType<Window*>(in);
        GE_ASSERT(_type);

        if((bool) (s.LoadMode & AssetLoadMode::Files))
            _asset = ref_cast((Asset*) _type->Factory(in, s.Window));
    }

    void File::ISerialize(ostream& out) const
    {
        GE_ASSERTM(!_asset.IsFree(), "Asset must be loaded.");
        GE_ASSERT(_type);
        WriteType(out, *_type);
        Write(out, *_asset);
    }

    Reference<Asset> File::Load() const
    {
        if(_asset) return _asset;
        return ForceLoad();
    }

    Reference<Asset> File::ForceLoad() const
    {
        GE_ASSERT(!_path.empty())
        GE_ASSERT(_type);

        if(_bank)
            return Load(_bank->MoveStreamToFile(GetUUID()));

        std::ifstream stream(_path, std::ios::in | std::ios::binary);

        if(!stream.is_open())
        {
            Log::Warning(std::format("Could not open \"{}\".", _path.string()));
            return DEFAULT;
        }

        return Load(stream);
    }

    Reference<Asset> File::Load(std::istream& in) const
    {
        GE_ASSERT(_type);
        GE_ASSERT(!_path.empty());

        if(_path.extension() == Extension)
        {
            Read<std::string>(in);
            Read<UUID>(in);
            ReadType<Window*>(in);
        }

        if(_asset)
            _asset->Deserialize(in, _window);
        else
            _asset = ref_cast((Asset*) _type->Factory(in, _window));

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
        return _asset = (Reference<Asset>) _weakAsset;
    }
}

const gE::File* ReadAssetReference(std::istream& in, gE::Window* window)
{
    return window->GetAssets().LoadFile(Read<std::string>(in), gE::AssetLoadMode::PathsAndFiles);
}