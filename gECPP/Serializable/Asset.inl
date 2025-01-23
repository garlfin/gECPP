#include "Asset.h"

namespace gE
{
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const Bank& a, const Bank& b) const { return a < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const Bank& a, const UUID& b) const { return a.GetUUID() < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const UUID& a, const Bank& b) const { return a < b.GetUUID(); }

    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const File& a, const File& b) const { return a < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const File& a, const UUID& b) const { return a.GetUUID() < b; }
    NODISCARD ALWAYS_INLINE bool AssetCompare::operator()(const UUID& a, const File& b) const { return a < b.GetUUID(); }
}