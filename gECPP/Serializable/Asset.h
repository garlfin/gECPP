//
// Created by scion on 11/9/2024.
//

#pragma once

#include <set>
#include <CityHash/CityCRC.h>
#include <Engine/Utility/AssetManager.h>
#include <Engine/Utility/Macro.h>
#include <Serializable/Serializable.h>

#ifdef DEBUG
    #define ASSET_CHECK_FREE(TYPE) if(!TYPE::IsFree()) { LOG("WARNING: ASSET NOT DELETED\n\tAsset: " << this); }
#else
    #define ASSET_CHECK_FREE(TYPE)
#endif

namespace gE
{
    class Bank;

    using UUID = __uint128_t;

    UUID HashPath(const Path& path);

    class Asset : public Serializable<Window*>
    {
    public:
        using SUPER = Serializable;

        Asset() = default;
        Asset(istream& in, SETTINGS_T s) : SUPER(in, s) { }

        DEFAULT_OPERATOR_CM(Asset);

        virtual void Free() = 0;
        NODISCARD virtual bool IsFree() const = 0;

        void Deserialize(ostream& out) const override { SUPER::Deserialize(out); }

        ~Asset() override = default;
    };

    enum class AssetLoadMode : u8
    {
        None = 0,
        Paths = 1,
        Files = 1 << 1,
        PathsAndFiles = Paths | Files,
    };

    ENUM_OPERATOR(AssetLoadMode, |);
    ENUM_OPERATOR(AssetLoadMode, &);

    struct FileLoadArgs
    {
        Reference<Bank> Bank;
        AssetLoadMode LoadMode;
    };

    class File final : public Serializable<FileLoadArgs&>
    {
        SERIALIZABLE_PROTO("FILE", 0, File, Serializable);

    public:
        File(const Path& path, const TypeSystem::Type* type);
        File(const Path& path, const Reference<Asset>& asset);
        File(const Path& path, const WeakReference<Asset>& asset);
        explicit File(const UUID& uuid) : _uuid(uuid) {}

        NODISCARD ALWAYS_INLINE const TypeSystem::Type* GetFileType() const { return _type; };
        NODISCARD ALWAYS_INLINE bool IsLoaded() const { return _weakAsset.IsValid(); }

        Reference<Asset> Load() const;
        NODISCARD Reference<Asset> LoadWeak() const;
        NODISCARD Reference<Asset> MakeWeak() const;
        Reference<Asset> Lock() const;

        GET_CONST(const Reference<Asset>&, , _asset);
        GET_CONST(const WeakReference<Asset>&, Weak, _weakAsset);
        GET_CONST(const Reference<Bank>&, Bank, _bank);

        bool operator<(const File& o) const { return _uuid < o._uuid; }

    private:
        static const TypeSystem::Type* ValidateAssetType(const Asset&);

        Path _path = DEFAULT;
        UUID _uuid = DEFAULT;
        Reference<Bank> _bank = DEFAULT;
        const TypeSystem::Type* _type = DEFAULT;

        mutable WeakReference<Asset> _weakAsset = DEFAULT;
        mutable Reference<Asset> _asset = DEFAULT;
    };

    struct BankLoadArgs
    {
        Path Path;
        std::ifstream Stream;
        Window* Window;
        AssetLoadMode Mode;
    };

    class Bank final : public Serializable<BankLoadArgs&>
    {
        SERIALIZABLE_PROTO("BANK", 0, Bank, Serializable);

    public:
        explicit Bank(Window* window, const Path& path, AssetLoadMode);

        void Free() { _path.clear(); _stream.close(); }
        bool IsFree() const { return _path.empty() && !_stream.is_open(); }

        GET_CONST(const Path&, Path, _path);
        GET(std::istream&, Stream, _stream);
        GET_CONST(Window&, Window, *_window);

       const File* LoadFile(const Path& path);
       const File* FindFile(const UUID& uuid);
       const File* FindFile(const Path& path);

        void UnloadFile(const File& file);
        bool UnloadFile(const Path& path);
        bool UnloadFile(const UUID& uuid);

        const File* AddFile(File&&);
        void RemoveFile(const File&);

    private:
        std::istream& MoveStreamToFile(const UUID& uuid);

        Path _path = DEFAULT;
        UUID _uuid = DEFAULT;
        std::ifstream _stream = DEFAULT;

        std::set<File> _files = DEFAULT;
        Window* _window = DEFAULT;

        friend class File;
    };

    enum class AssetSearchMode
    {
        Banks = 1,
        Files = 1 << 1,
        BanksAndFiles = Banks | Files
    };

    ENUM_OPERATOR(AssetSearchMode, |);
    ENUM_OPERATOR(AssetSearchMode, &);

    class FileManager
    {
    public:
        explicit FileManager(Window* window) : _window(window) {}

        void LoadBanksInFolder(const Path& folder);

        Bank* LoadBank(const Path& path, AssetLoadMode = AssetLoadMode::Paths);
        Bank* FindBank(const Path& path, AssetLoadMode = AssetLoadMode::Paths);

        void UnloadBank(Bank&);
        bool UnloadBank(const Path& path);
        void RemoveBank(Bank&);
        bool RemoveBank(const Path& path);

        File* LoadFile(const Path& path, AssetSearchMode = AssetSearchMode::Banks);
        File* FindFile(const Path& path, AssetSearchMode = AssetSearchMode::Banks);
        bool UnloadFile(const Path& path);
        bool RemoveFile(const Path& path);

    private:
        Window* _window;

        std::set<File> _files = DEFAULT;
        std::set<Bank> _banks = DEFAULT;
    };
}
