//
// Created by scion on 11/9/2024.
//

#pragma once

#include <map>
#include <set>
#include <Core/Macro.h>
#include <Core/Pointer.h>
#include <Core/UUID.h>
#include <Core/Serializable/Serializable.h>

#ifdef DEBUG
    #define ASSET_CHECK_FREE(TYPE) if(!TYPE::IsFree()) { gE::Log::Write("INFO: ASSET NOT DELETED BEFORE DESTRUCTOR\n\tAsset: {:x}\n", (u64) this); }
#else
    #define ASSET_CHECK_FREE(TYPE)
#endif

namespace gE
{
    class Bank;
    class File;

    Path FixPath(const Path& path);
    Path FixPath(const Path& path, const Type<Window*>& type);

    class Asset : public Serializable<Window*>
    {
        REFLECTABLE_NOIMPL(Serializable);

    public:
        using SUPER = Serializable;

        Asset() = default;
        Asset(istream& in, SETTINGS_T s) : SUPER(in, s) { }

        DEFAULT_OPERATOR_CM(Asset);

        virtual void Free() = 0;
        NODISCARD virtual bool IsFree() const = 0;

        GET_CONST(File*, File, _file);

        void Serialize(ostream& out) const override { SUPER::Serialize(out); }

        ~Asset() override = default;

        friend class File;

    private:
        File* _file = nullptr;
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

    struct FileInfo final : public Serializable<>
    {
        SERIALIZABLE_PROTO_NOHEADER("INFO", FileInfo, Serializable);

    public:
        FileInfo(const Path& path, UUID uuid, size_t offset) : Path(path), UUID(uuid), Offset(offset) {};
        FileInfo(Path&& path, UUID uuid, size_t offset) : Path(std::move(path)), UUID(uuid), Offset(offset) {};

        Path Path;
        UUID UUID;
        mutable size_t Offset;
    };

    struct FileLoadArgs
    {
        Window* Window;
        Bank* Bank;
        Path Path;
        AssetLoadMode LoadMode;
    };

    struct BankLoadArgs
    {
        Path Path;
        Reference<std::istream> Stream;
        Window* Window;
        AssetLoadMode Mode;
    };

    struct SerializableCompare
    {
        using is_transparent = void;

        NODISCARD ALWAYS_INLINE bool operator()(const ISerializable& a, const ISerializable& b) const;
        NODISCARD ALWAYS_INLINE bool operator()(const ISerializable& a, const UUID& b) const;
        NODISCARD ALWAYS_INLINE bool operator()(const UUID& a, const ISerializable& b) const;

        NODISCARD ALWAYS_INLINE bool operator()(const FileInfo& a, const FileInfo& b) const;
        NODISCARD ALWAYS_INLINE bool operator()(const FileInfo& a, const UUID& b) const;
        NODISCARD ALWAYS_INLINE bool operator()(const UUID& a, const FileInfo& b) const;
    };

}

template struct TypeSystem<const gE::FileLoadArgs&>;
template struct TypeSystem<const gE::BankLoadArgs&>;

namespace gE
{
    class File final : public Serializable<const FileLoadArgs&>
    {
        SERIALIZABLE_PROTO_ABSTRACT("FILE", 0, File, Serializable);
        REFLECTABLE_NAME_PROTO();

        OPERATOR_MOVE_PROTO(File);
        DELETE_OPERATOR_COPY(File);

    public:
        File(Window* window, const Path& path, const Type<Window*>* type);
        File(Window* window, const Path& path, const Reference<Asset>& asset);
        File(Window* window, const Path& path, const WeakReference<Asset>& asset);

        template<class T> requires std::is_base_of_v<Asset, T>
        File(Window* window, const Path& path, T&& t);

        NODISCARD ALWAYS_INLINE const Type<Window*>* GetFileType() const { return _type; };
        NODISCARD ALWAYS_INLINE bool IsLoaded() const { return _weakAsset.IsValid(); }

        Reference<Asset> Load() const;
        Reference<Asset> ForceLoad() const;
        NODISCARD Reference<Asset> LoadWeak() const;
        NODISCARD Reference<Asset> MakeWeak() const;
        Reference<Asset> Lock() const;

        template<class T, bool SAFE> requires std::is_base_of_v<Asset, T>
        NODISCARD ALWAYS_INLINE Reference<T> Cast() const;

        GET_CONST(const Reference<Asset>&, , _asset);
        GET_CONST(const WeakReference<Asset>&, Weak, _weakAsset);
        GET_CONST(Bank*, Bank, _bank);
        GET_CONST(const Path&, Path, _path);
        GET_CONST(Window&, Window, *_window);

        FORCE_IMPL static inline const Path Extension = ".file";

    private:
        static const Type<Window*>* ValidateAssetType(const Asset&);

        NODISCARD Reference<Asset> Load(std::istream&) const;

        Window* _window = DEFAULT;
        const Type<Window*>* _type = DEFAULT;
        Path _path = DEFAULT;
        Bank* _bank = DEFAULT;

        mutable WeakReference<Asset> _weakAsset = DEFAULT;
        mutable Reference<Asset> _asset = DEFAULT;

        using Serializable::GetType;
        using Serializable::GetUnderlying;
        using Serializable::OnEditorGUI;
        using Serializable::OnEditorIcon;

        friend class Bank;
    };

    class Bank final : public Serializable<const BankLoadArgs&>
    {
        SERIALIZABLE_PROTO("BANK", 0, Bank, Serializable);
        REFLECTABLE_NAME_PROTO();

    public:
        using FILE_SET_T = std::set<File, SerializableCompare>;
        using FILE_INFO_SET_T = std::set<FileInfo, SerializableCompare>;

        Bank(Window* window, const Path& path, AssetLoadMode);

        void Free() { _path.clear(); _stream.Free(); _fileInfo.clear(); }
        bool IsFree() const { return _path.empty() && !_stream && _fileInfo.empty(); }

        GET_CONST(const Path&, Path, _path);
        GET(std::istream&, Stream, _stream);
        GET_CONST(Window&, Window, *_window);
        GET_CONST(const UUID&, UUID, _uuid);

        const File* FindFile(const Path& path) const { return FindFile(HashPath(path)); }
        const File* FindFile(const UUID& uuid) const;

        const File* AddFile(File&&);
        void RemoveFile(const File&);

        template<class SERIALIZABLE_T>
        const File* AddSerializableFromFile(const Path&);

        NODISCARD ALWAYS_INLINE bool operator<(const Bank& o) const { return _uuid < o._uuid; }

    private:
        std::istream& MoveStreamToFile(const UUID& uuid);

        Window* _window = DEFAULT;

        Path _path = DEFAULT;
        UUID _uuid = DEFAULT;
        Reference<std::istream> _stream = DEFAULT;
        FILE_SET_T _files = DEFAULT;
        FILE_INFO_SET_T _fileInfo = DEFAULT;

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
        using FILE_SET_T = std::set<File, SerializableCompare>;
        using BANK_SET_T = std::set<Pointer<Bank>, SerializableCompare>;

        explicit AssetManager(Window* window) : _window(window) {}

        void LoadBanksInFolder(const Path& folder, AssetLoadMode mode = AssetLoadMode::Paths);

        Bank* LoadBank(const Path& path, AssetLoadMode = AssetLoadMode::Paths);
        NODISCARD Bank* FindBank(const Path& path) { return FindBank(HashPath(path)); }
        NODISCARD Bank* FindBank(const UUID& uuid);

        const File* LoadFile(const Path& path, AssetLoadMode = AssetLoadMode::Paths);
        const File* AddFile(File&&);

        template<class SERIALIZABLE_T> requires ReflConstructible<SERIALIZABLE_T>
        const File* AddSerializableFromFile(const Path&);

        NODISCARD const File* FindFile(const Path& path) const { return FindFile(HashPath(path)); }
        NODISCARD const File* FindFile(const UUID& uuid) const;

        void RemoveBank(const Bank& bank) { RemoveBank(bank.GetUUID()); }
        bool RemoveBank(const Path& path) { return RemoveBank(HashPath(path)); }
        bool RemoveBank(const UUID& uuid);

        bool RemoveFile(const File& file) { return RemoveFile(file.GetUUID()); }
        bool RemoveFile(const Path& path) { return RemoveFile(HashPath(path)); }
        bool RemoveFile(const UUID& uuid);

        GET(FILE_SET_T&, Files, _files);
        GET(BANK_SET_T&, Banks, _banks);

    private:
        Window* _window;

        FILE_SET_T _files = DEFAULT;
        BANK_SET_T _banks = DEFAULT;
    };
}

template<class T> requires std::is_base_of_v<T, gE::Asset>
void Read(std::istream&, gE::Window*, Reference<T>&);

const gE::File* ReadAssetReference(std::istream&, gE::Window*);

template <class T> requires std::is_base_of_v<T, gE::Asset>
void Write(std::ostream&, const Reference<T>&);

#include "Asset.inl"