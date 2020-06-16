#pragma once

#include "Pit.h"
#include "Textures.h"

#include "je/Batch.h"

class PitRenderer
{
public:
    PitRenderer(const Pit& apit, const Textures& atextures, je::Batch& batch)
        : pit_{apit}, textures_{atextures}, batch{batch}
    {
    }

    void Draw(je::Vec2f topLeft, float internalTileScroll, const float bottomRow);
    void DrawContents(je::Vec2f topLeft, float internalTileScroll, const float bottomRow);
    void DrawOutline(je::Vec2f topLeft);
    const int HeightAt(size_t col, size_t row) const;
    const je::TextureRegion* TileAt(size_t col, size_t row) const;

private:
    const Pit& pit_;
    const Textures& textures_;
    je::Batch& batch;
};
