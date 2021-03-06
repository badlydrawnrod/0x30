#include "PitRenderer.h"

#include "Pit.h"
#include "je/Logger.h"
#include "je/QuadHelpers.h"

void PitRenderer::Draw(je::Vec2f topLeft, float internalTileScroll, const float bottomRow)
{
    DrawContents(topLeft, internalTileScroll, bottomRow);
    DrawOutline(topLeft);
}

void PitRenderer::DrawContents(je::Vec2f topLeft, float internalTileScroll, const float bottomRow)
{
    // Draw the contents of the pit.
    for (size_t row = 0; row < Pit::rows; row++)
    {
        for (size_t col = 0; col < Pit::cols; col++)
        {
            const je::TextureRegion* drawTile = TileAt(col, row);
            const int heightAt = HeightAt(col, row);
            if (drawTile)
            {
                float y = topLeft.y + row * drawTile->h - internalTileScroll;
                if (row == 0)
                {
                    // Clip the top row to the top of the pit.
                    batch.AddVertices(je::quads::Create(
                            *drawTile,
                            topLeft.x + col * drawTile->w,
                            topLeft.y,
                            0.0f,
                            internalTileScroll,
                            drawTile->w,
                            drawTile->h - internalTileScroll - heightAt));
                }
                else if (row < Pit::rows - 1)
                {
                    batch.AddVertices(je::quads::Create(
                            *drawTile,
                            topLeft.x + col * drawTile->w,
                            topLeft.y + row * drawTile->h - internalTileScroll - heightAt));
                }
                else if (y < bottomRow)
                {
                    // Fade the row in.
                    GLubyte c = static_cast<GLubyte>(0xff * (internalTileScroll / drawTile->h));
                    if (c < 0x7f)
                    {
                        c = 0x7f;
                    }
                    je::Rgba4b grey{c, c, c, 0xff};

                    // Clip the bottom row to the bottom of the pit.
                    batch.AddVertices(je::quads::Create(
                            *drawTile,
                            topLeft.x + col * drawTile->w,
                            topLeft.y + row * drawTile->h - internalTileScroll - heightAt,
                            0.0f,
                            0.0f,
                            drawTile->w,
                            internalTileScroll,
                            grey));
                }
            }
        }
    }
}

int PitRenderer::HeightAt(size_t col, size_t row) const
{
    return pit_.HeightAt(col, row);
}

const je::TextureRegion* PitRenderer::TileAt(size_t col, size_t row) const
{
    switch (pit_.TileTypeAt(col, row))
    {
    case Pit::TileType::Red:
        return &textures_.redTile;

    case Pit::TileType::Yellow:
        return &textures_.yellowTile;

    case Pit::TileType::Green:
        return &textures_.greenTile;

    case Pit::TileType::Cyan:
        return &textures_.cyanTile;

    case Pit::TileType::Magenta:
        return &textures_.magentaTile;

    case Pit::TileType::Blue:
        return &textures_.blueTile;

    case Pit::TileType::Wall:
        return &textures_.wallTile;

    case Pit::TileType::None:
    default:
        return nullptr;
    }
}

void PitRenderer::DrawOutline(je::Vec2f topLeft)
{
    // Draw the outline of the pit.
    const auto& wallTile = textures_.wallTile;

    // Left and right sides.
    for (size_t y = 0; y < Pit::rows * 2 - 2; y++)
    {
        batch.AddVertices(je::quads::Create(textures_.pitLeft, topLeft.x - textures_.pitLeft.w, topLeft.y + textures_.pitLeft.h * y));
        batch.AddVertices(je::quads::Create(textures_.pitRight, topLeft.x + Pit::cols * wallTile.w, topLeft.y + textures_.pitRight.h * y));
    }

    // Top and bottom sides.
    for (size_t x = 1; x < Pit::cols * 2 + 1; x++)
    {
        batch.AddVertices(je::quads::Create(textures_.pitTop, topLeft.x - textures_.pitTop.w + x * textures_.pitTop.w, topLeft.y - textures_.pitTop.h));
        batch.AddVertices(je::quads::Create(textures_.pitBottom, topLeft.x - textures_.pitBottom.w + x * textures_.pitBottom.w, topLeft.y + wallTile.h * (Pit::rows - 1)));
    }

    // Corners.
    batch.AddVertices(je::quads::Create(textures_.pitTopLeft, topLeft.x - textures_.pitTopLeft.w, topLeft.y - textures_.pitTopLeft.h));
    batch.AddVertices(je::quads::Create(textures_.pitTopRight, topLeft.x + (2 * Pit::cols) * textures_.pitTopRight.w, topLeft.y - textures_.pitTopRight.h));
    batch.AddVertices(je::quads::Create(textures_.pitBottomLeft, topLeft.x - textures_.pitBottomLeft.w, topLeft.y + (2 * Pit::rows - 2) * textures_.pitBottomLeft.h));
    batch.AddVertices(je::quads::Create(textures_.pitBottomRight, topLeft.x + (2 * Pit::cols) * textures_.pitBottomRight.w, topLeft.y + (2 * Pit::rows - 2) * textures_.pitBottomRight.h));
}
