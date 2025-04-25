//
// Created by scion on 3/4/2025.
//

#pragma once

#include <Graphics/Texture/Texture.h>

namespace gE::Editor
{
    struct SpriteSheet
    {
    public:
        API::Texture2D Texture;
        u16vec2 SpriteCount;

        vec2 GetSpriteSize() const { return 1.f / (vec2) SpriteCount; }
        vec2 GetBottomUV(u16vec2 sprite) const { return GetSpriteSize() * (vec2) sprite; };
        vec2 GetTopUV(u16vec2 sprite) const { return GetBottomUV(sprite) + GetSpriteSize(); };
    };

    struct Sprite
    {
    public:
        Reference<SpriteSheet> SpriteSheet;
        u16vec2 SpriteID;

        vec2 GetBottomUV() const { return SpriteSheet->GetBottomUV(SpriteID); };
        vec2 GetTopUV() const { return SpriteSheet->GetTopUV(SpriteID); };
    };
}