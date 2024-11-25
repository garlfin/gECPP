//
// Created by scion on 11/9/2024.
//

#pragma once

#include <Serializable/Serializable.h>

#ifdef DEBUG
    #define ASSET_CHECK_FREE(TYPE) if(!TYPE::IsFree()) { LOG("WARNING: ASSET NOT DELETED\n\tAsset: " << this); }
#else
    #define ASSET_CHECK_FREE(TYPE)
#endif

namespace gE
{
    class Asset
    {
    public:
        Asset() = default;

        DEFAULT_OPERATOR_CM(Asset);

        virtual void Free() = 0;
        NODISCARD virtual bool IsFree() const = 0;

        virtual ~Asset() = default;
    };
}
