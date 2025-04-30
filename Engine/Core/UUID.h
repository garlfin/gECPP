//
// Created by scion on 4/30/2025.
//

#pragma once

namespace gE
{
    using UUID = __uint128_t;

    UUID HashPath(const Path& path);
    UUID GenerateUUID();
}