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
        glm::u16vec2 SpriteCount;

        glm::vec2 GetSpriteSize() const { return 1.f / (glm::vec2) SpriteCount; }
        glm::vec2 GetBottomUV(glm::u16vec2 sprite) const { return GetSpriteSize() * (glm::vec2) sprite; };
        glm::vec2 GetTopUV(glm::u16vec2 sprite) const { return GetBottomUV(sprite) + GetSpriteSize(); };
    };

    struct Sprite
    {
    public:
        Reference<SpriteSheet> SpriteSheet;
        glm::u16vec2 SpriteID;

        glm::vec2 GetBottomUV() const { return SpriteSheet->GetBottomUV(SpriteID); };
        glm::vec2 GetTopUV() const { return SpriteSheet->GetTopUV(SpriteID); };
    };
}