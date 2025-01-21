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

        NODISCARD ALWAYS_INLINE const TypeSystem::Type* GetFileType() const { return _type; };

        NODISCARD Reference<Asset> Load();
        NODISCARD ALWAYS_INLINE bool IsLoaded() const { return _asset.IsValid(); }

    private:
        static const TypeSystem::Type* ValidateAssetType(const Asset&);

        Path _path = DEFAULT;
        Reference<std::istream> _bank = DEFAULT;
        const TypeSystem::Type* _type = DEFAULT;
        WeakReference<Asset> _asset = DEFAULT;
    };

    class Bank
    {
    public:
        Bank() = default;

    private:
        Path _path;
    };

    class FileManager
    {
    public:
        FileManager() = default;

        Reference<Bank> LoadBank(const Path&, bool loadFiles);
        Reference<File> LoadFile();
        Reference<File> LoadFile(const Path&);

    private:
        std::set<File> _files;
        std::vector<Bank> _banks;
    };
}
