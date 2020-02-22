#include "PitRenderer.h"

#include "Pit.h"
#include "je/QuadHelpers.h"


void PitRenderer::Draw(je::Vec2f topLeft, float internalTileScroll, const float lastRow, const float bottomRow)
{
    DrawContents(topLeft, internalTileScroll, lastRow, bottomRow);
    DrawOutline(topLeft);
}


void PitRenderer::DrawContents(je::Vec2f topLeft, float internalTileScroll, const float lastRow, const float bottomRow)
{
    // Draw the contents of the pit.
    for (auto row = 0; row < Pit::rows; row++)
    {
        for (auto col = 0; col < Pit::cols; col++)
        {
            const je::TextureRegion* drawTile = TileAt(col, row);
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
                        drawTile->h - internalTileScroll
                        ));
                }
                else if (row < Pit::rows - 1)
                {
                    batch.AddVertices(je::quads::Create(*drawTile, topLeft.x + col * drawTile->w, topLeft.y + row * drawTile->h - internalTileScroll));
                }
                else if (y < bottomRow)
                {
                    // TODO: clip, so we only draw the top part of the tile that's visible in the pit.
                    // Fade in the last row.
                    GLubyte c = (GLubyte)0x3f;
                    switch (int(internalTileScroll))
                    {
                    case 15:
                    case 14:
                        c = (GLubyte)0xef;
                        break;
                    case 13:
                    case 12:
                        c = (GLubyte)0xdf;
                        break;
                    case 11:
                    case 10:
                        c = (GLubyte)0xcf;
                        break;
                    case 9:
                    case 8:
                        c = (GLubyte)0xbf;
                        break;
                    case 7:
                    case 6:
                        c = (GLubyte)0xaf;
                        break;
                    case 5:
                    case 4:
                        c = (GLubyte)0x9f;
                        break;
                    case 3:
                    case 2:
                        c = (GLubyte)0x8f;
                        break;
                    case 1:
                        c = (GLubyte)0x7f;
                        break;
                    default:
                        c = (GLubyte)0xff;
                        break;
                    }
                    je::Rgba4b grey{ c, c, c, 0xff };

                    // Clip the bottom row to the bottom of the pit.
                    batch.AddVertices(je::quads::Create(
                        *drawTile,
                        topLeft.x + col * drawTile->w,
                        topLeft.y + row * drawTile->h - internalTileScroll,
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


const je::TextureRegion* PitRenderer::TileAt(size_t col, size_t row) const
{
    switch (pit_.TileAt(col, row))
    {
    case Pit::Tile::Red:
        return &textures_.redTile;

    case Pit::Tile::Yellow:
        return &textures_.yellowTile;

    case Pit::Tile::Green:
        return &textures_.greenTile;

    case Pit::Tile::Cyan:
        return &textures_.cyanTile;

    case Pit::Tile::Magenta:
        return &textures_.magentaTile;

    case Pit::Tile::Wall:
        return &textures_.wallTile;

    case Pit::Tile::None:
    default:
        return nullptr;
    }
}


void PitRenderer::DrawOutline(je::Vec2f topLeft)
{
    // Draw the outline of the pit.
    const auto& wallTile = textures_.wallTile;
    for (int y = 0; y < Pit::rows * 2 - 2; y++)
    {
        batch.AddVertices(je::quads::Create(textures_.pitLeft, topLeft.x - textures_.pitLeft.w, topLeft.y + textures_.pitLeft.h * y));
        batch.AddVertices(je::quads::Create(textures_.pitRight, topLeft.x + Pit::cols * wallTile.w, topLeft.y + textures_.pitRight.h * y));
    }
    for (int x = 1; x < Pit::cols * 2 + 1; x++)
    {
        batch.AddVertices(je::quads::Create(textures_.pitTop, topLeft.x - textures_.pitTop.w + x * textures_.pitTop.w, topLeft.y - textures_.pitTop.h));
        batch.AddVertices(je::quads::Create(textures_.pitBottom, topLeft.x - textures_.pitBottom.w + x * textures_.pitBottom.w, topLeft.y + wallTile.h * (Pit::rows - 1)));
    }
}
