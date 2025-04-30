#pragma once

#include "Asset.h"

namespace gE
{
    NODISCARD ALWAYS_INLINE bool SerializableCompare::operator()(const ISerializable& a, const ISerializable& b) const { return a.GetUUID() < b.GetUUID(); }
    NODISCARD ALWAYS_INLINE bool SerializableCompare::operator()(const ISerializable& a, const UUID& b) const { return a.GetUUID() < b; }
    NODISCARD ALWAYS_INLINE bool SerializableCompare::operator()(const UUID& a, const ISerializable& b) const { return a < b.GetUUID(); }

    NODISCARD ALWAYS_INLINE bool SerializableCompare::operator()(const FileInfo& a, const FileInfo& b) const { return a.UUID < b.UUID; }
    NODISCARD ALWAYS_INLINE bool SerializableCompare::operator()(const FileInfo& a, const UUID& b) const { return a.UUID < b; }
    NODISCARD ALWAYS_INLINE bool SerializableCompare::operator()(const UUID& a, const FileInfo& b) const { return a < b.UUID; }

    template <class T> requires std::is_base_of_v<Asset, T>
    File::File(Window* window, const Path& path, T&& t) : File(window, path, ref_create<T>(std::move(t))) {}

    template <class T, bool SAFE> requires std::is_base_of_v<Asset, T>
    Reference<T> File::Cast() const
    {
        if(!_weakAsset.IsValid()) return DEFAULT;

        if constexpr(SAFE)
            return _asset->IsCastable<T>() ? (Reference<T>) _asset : Reference<T>();
        else
            return (Reference<T>) _asset;
    }

    template <class SERIALIZABLE_T>
    const File* Bank::AddSerializableFromFile(const Path& path)
    {
        return AddFile(File(path, ref_cast(ReadSerializableFromFile<SERIALIZABLE_T>(_window, path))));
    }

    template <class SERIALIZABLE_T> requires ReflConstructible<SERIALIZABLE_T>
    const File* AssetManager::AddSerializableFromFile(const Path& path)
    {
        const File* file = AddFile(File(_window, path, &SERIALIZABLE_T::SType));
        file->Load();
        return file;
    }
}

template<class T> requires std::is_base_of_v<T, gE::Asset>
void Read(std::istream& in, gE::Window* window, Reference<T>& reference)
{
    reference = DEFAULT;
    if(Read<bool>(in))
        reference = ReadAssetReference(in, window)->Cast<T, false>();
}

template <class T> requires std::is_base_of_v<T, gE::Asset>
void Write(std::ostream& out, const Reference<T>& reference)
{
    Write(out, (bool) reference->GetFile());
    if(reference->GetFile())
        Write(out, reference->GetFile().GetPath().string());
}