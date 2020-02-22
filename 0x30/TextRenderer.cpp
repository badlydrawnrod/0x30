#include "TextRenderer.h"

#include "je/Batch.h"
#include "je/QuadHelpers.h"
#include "je/Types.h"


TextRenderer::TextRenderer(const je::TextureRegion& tiles, je::Batch& batch, float tileWidth, float tileHeight)
    : tiles_{ tiles }, batch_{ batch }, tileWidth_{ tileWidth }, tileHeight_{ tileHeight }
{
}


void TextRenderer::Draw(float x, float y, const std::string& text, const je::Rgba4b colour)
{
    Draw(x, y, text.c_str(), colour);
}


void TextRenderer::Draw(float x, float y, const char* text, const je::Rgba4b colour)
{
    // Sanity.
    if (!text)
    {
        return;
    }

    int widthInTiles = static_cast<int>(tiles_.w / tileWidth_);
    for (const char* s = text; *s != '\0'; s++)
    {
        // Reject out of range.
        if (*s < ' ' || *s > '~')
        {
            continue;
        }

        const int c = *s - ' ';
        const float srcX = (c % widthInTiles) * tileWidth_;
        const float srcY = tiles_.y + (c / widthInTiles) * tileHeight_;
        batch_.AddVertices(je::quads::Create(
            tiles_.texture,
            x, y,
            srcX, srcY, tileWidth_, tileHeight_,
            colour
        ));
        x += tileWidth_;
    }
}
