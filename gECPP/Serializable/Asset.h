//
// Created by scion on 11/9/2024.
//

#pragma once

#include <Engine/Utility/Macro.h>
#include <Serializable/Serializable.h>
#include <Engine/Utility/AssetManager.h>
#include <set>

#ifdef DEBUG
    #define ASSET_CHECK_FREE(TYPE) if(!TYPE::IsFree()) { LOG("WARNING: ASSET NOT DELETED\n\tAsset: " << this); }
#else
    #define ASSET_CHECK_FREE(TYPE)
#endif

namespace gE
{
    class Bank;

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

    class File final : public Serializable<Reference<Bank>>
    {
        SERIALIZABLE_PROTO("FILE", 0, File, Serializable);

    public:
        explicit File(const Path& path, const TypeSystem::Type* type);
        explicit File(const Path& path, const Reference<Asset>& asset);
        explicit File(const Path& path, const WeakReference<Asset>& asset);

        NODISCARD ALWAYS_INLINE const TypeSystem::Type* GetFileType() const { return _type; };
        NODISCARD ALWAYS_INLINE bool IsLoaded() const { return _weakAsset.IsValid(); }

        Reference<Asset> Load();
        NODISCARD Reference<Asset> LoadWeak();

        NODISCARD Reference<Asset> MakeWeak();
        bool Lock();

        GET(Reference<Asset>&, , _asset);
        GET(WeakReference<Asset>, Weak, _weakAsset);

    private:
        static const TypeSystem::Type* ValidateAssetType(const Asset&);

        Path _path = DEFAULT;
        Reference<Bank> _bank = DEFAULT;
        const TypeSystem::Type* _type = DEFAULT;

        WeakReference<Asset> _weakAsset = DEFAULT;
        Reference<Asset> _asset = DEFAULT;
    };

    class Bank
    {
    public:
        Bank() = default;
        explicit Bank(Window* window, const Path& path);

        void Free() { _path.clear(); _stream.close(); }
        bool IsFree() const { return _path.empty() && !_stream.is_open(); }

        GET_CONST(const Path&, Path, _path);
        GET(std::istream&, Stream, _stream);
        GET_CONST(Window&, Window, *_window);

    private:
        Path _path;
        std::ifstream _stream;
        Window* _window;
    };

    class FileManager
    {
    public:
        FileManager() = default;

        void LoadBanksInFolder(const Path& folder);

        Reference<Bank> LoadBank(const Path& path);
        Reference<Bank> FindBank(const Path& path);

        File& LoadFile(const Path& path);
        File& FindFile(const Path& path);

    private:
        std::set<File> _files;
        std::vector<Bank> _banks;
    };
}
