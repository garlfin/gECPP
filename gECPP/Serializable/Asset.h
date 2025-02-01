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

        void Serialize(ostream& out) const override { SUPER::Serialize(out); }

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
        Window* Window;
        Bank* Bank;
        AssetLoadMode LoadMode;
    };

    class File final : public Serializable<FileLoadArgs&>
    {
        SERIALIZABLE_PROTO("FILE", 0, File, Serializable);

    public:
        File(const Path& path, const TypeSystem::Type* type);
        File(const Path& path, const Reference<Asset>& asset);
        File(const Path& path, const WeakReference<Asset>& asset);

        NODISCARD ALWAYS_INLINE const TypeSystem::Type* GetFileType() const { return _type; };
        NODISCARD ALWAYS_INLINE bool IsLoaded() const { return _weakAsset.IsValid(); }

        Reference<Asset> Load() const;
        NODISCARD Reference<Asset> LoadWeak() const;
        NODISCARD Reference<Asset> MakeWeak() const;
        Reference<Asset> Lock() const;

        template<class T> requires std::is_base_of_v<Asset, T>
        NODISCARD ALWAYS_INLINE T& Cast() const;

        template<class T> requires std::is_base_of_v<Asset, T>
        NODISCARD ALWAYS_INLINE T& CastSafe() const;

        GET_CONST(const Reference<Asset>&, , _asset);
        GET_CONST(const WeakReference<Asset>&, Weak, _weakAsset);
        GET_CONST(Bank*, Bank, _bank);
        GET_CONST(const UUID&, UUID, _uuid);
        GET_CONST(const Path&, Path, _path);

        NODISCARD ALWAYS_INLINE bool operator<(const File& o) const { return _uuid < o._uuid; }

    private:
        static const TypeSystem::Type* ValidateAssetType(const Asset&);

        Path _path = DEFAULT;
        UUID _uuid = DEFAULT;
        Bank* _bank = DEFAULT;
        const TypeSystem::Type* _type = DEFAULT;

        mutable WeakReference<Asset> _weakAsset = DEFAULT;
        mutable Reference<Asset> _asset = DEFAULT;

        friend class Bank;
    };

    struct BankLoadArgs
    {
        Path Path;
        std::ifstream Stream;
        Window* Window;
        AssetLoadMode Mode;
    };

    struct AssetCompare
    {
        using is_transparent = void;

        NODISCARD ALWAYS_INLINE bool operator()(const Bank& a, const Bank& b) const;
        NODISCARD ALWAYS_INLINE bool operator()(const Bank& a, const UUID& b) const;
        NODISCARD ALWAYS_INLINE bool operator()(const UUID& a, const Bank& b) const;

        NODISCARD ALWAYS_INLINE bool operator()(const File& a, const File& b) const;
        NODISCARD ALWAYS_INLINE bool operator()(const File& a, const UUID& b) const;
        NODISCARD ALWAYS_INLINE bool operator()(const UUID& a, const File& b) const;
    };

    class Bank final : public Serializable<BankLoadArgs&>
    {
        SERIALIZABLE_PROTO("BANK", 0, Bank, Serializable);

    public:
        Bank(Window* window, const Path& path, AssetLoadMode);

        void Free() { _path.clear(); _stream.close(); }
        bool IsFree() const { return _path.empty() && !_stream.is_open(); }

        GET_CONST(const Path&, Path, _path);
        GET(std::istream&, Stream, _stream);
        GET_CONST(Window&, Window, *_window);
        GET_CONST(const UUID&, UUID, _uuid);

        const File* FindFile(const Path& path) { return FindFile(HashPath(path)); }
        const File* FindFile(const UUID& uuid);

        const File* AddFile(File&&);
        void RemoveFile(const File&);

        NODISCARD ALWAYS_INLINE bool operator<(const Bank& o) const { return _uuid < o._uuid; }

    private:
        std::istream& MoveStreamToFile(const UUID& uuid);

        Path _path = DEFAULT;
        UUID _uuid = DEFAULT;
        std::ifstream _stream = DEFAULT;

        std::set<File, AssetCompare> _files = DEFAULT;
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

    class AssetManager
    {
    public:
        explicit AssetManager(Window* window) : _window(window) {}

        void LoadBanksInFolder(const Path& folder, AssetLoadMode mode = AssetLoadMode::Paths);

        Bank* LoadBank(const Path& path, AssetLoadMode = AssetLoadMode::Paths);
        NODISCARD Bank* FindBank(const Path& path) { return FindBank(HashPath(path)); }
        NODISCARD Bank* FindBank(const UUID& uuid);

        const File* LoadFile(const Path& path, AssetLoadMode = AssetLoadMode::Paths);
        const File* AddFile(File&&);

        NODISCARD const File* FindFile(const Path& path) { return FindFile(HashPath(path)); }
        NODISCARD const File* FindFile(const UUID& uuid);

        void RemoveBank(const Bank& bank) { RemoveBank(bank.GetUUID()); }
        bool RemoveBank(const Path& path) { return RemoveBank(HashPath(path)); }
        bool RemoveBank(const UUID& uuid);

        bool RemoveFile(const File& file) { return RemoveFile(file.GetUUID()); }
        bool RemoveFile(const Path& path) { return RemoveFile(HashPath(path)); }
        bool RemoveFile(const UUID& uuid);

    private:
        Window* _window;

        std::set<File, AssetCompare> _files = DEFAULT;
        std::set<Pointer<Bank>, AssetCompare> _banks = DEFAULT;
    };
}

#include "Asset.inl"