#include "Asset.h"

namespace gE
{
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const Bank& a, const Bank& b) const { return a < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const Bank& a, const UUID& b) const { return a.GetUUID() < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const UUID& a, const Bank& b) const { return a < b.GetUUID(); }

    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const File& a, const File& b) const { return a < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const File& a, const UUID& b) const { return a.GetUUID() < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const UUID& a, const File& b) const { return a < b.GetUUID(); }

    template <class T> requires std::is_base_of_v<Asset, T>
    T& File::Cast() const
    {
        GE_ASSERT(_weakAsset.IsValid());
        return *(T*) _asset.GetPointer();
    }

    template <class T> requires std::is_base_of_v<Asset, T>
    T& File::CastSafe() const
    {
        GE_ASSERT(_weakAsset.IsValid());
        return dynamic_cast<T*>(_asset.GetPointer());
    }
}
