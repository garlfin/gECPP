#pragma once

#include "Asset.h"

namespace gE
{
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const Bank& a, const Bank& b) const { return a < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const Bank& a, const UUID& b) const { return a.GetUUID() < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const UUID& a, const Bank& b) const { return a < b.GetUUID(); }

    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const File& a, const File& b) const { return a < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const File& a, const UUID& b) const { return a.GetUUID() < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const UUID& a, const File& b) const { return a < b.GetUUID(); }

    template <class T, bool SAFE> requires std::is_base_of_v<Asset, T>
    Reference<T> File::Cast() const
    {
        GE_ASSERT(_weakAsset.IsValid());

        if constexpr(SAFE)
            return dynamic_cast<T*>(_asset.GetPointer()) ? (Reference<T>) _asset : Reference<T>();
        else
            return (Reference<T>) _asset;
    }

    template <class T, bool SAFE> requires std::is_base_of_v<Asset, T>
    WeakReference<T> File::CastWeak() const
    {
        if constexpr(SAFE)
            return _weakAsset.IsValid() && dynamic_cast<T*>(_weakAsset.GetPointer()) ? (WeakReference<T>) _weakAsset : WeakReference<T>();
        else
            return (WeakReference<T>) _weakAsset;
    }

    template <class SERIALIZABLE_T>
    const File* Bank::AddSerializableFromFile(const Path& path)
    {
        return AddFile(File(path, ref_cast(ReadSerializableFromFile<SERIALIZABLE_T>(_window, path))));
    }

    template <class SERIALIZABLE_T>
    const File* AssetManager::AddSerializableFromFile(const Path& path)
    {
        return AddFile(File(path, ref_cast(ReadSerializableFromFile<SERIALIZABLE_T>(_window, path))));
    }
}
