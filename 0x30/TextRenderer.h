#pragma once

#include "je/Batch.h"
#include "je/Types.h"

#include <string>


class TextRenderer
{
public:
    TextRenderer(const je::TextureRegion& tiles, je::Batch& batch, float tileWidth = 8.0f, float tileHeight = 8.0f);

    void DrawLeft(float x, float y, const std::string& text, const je::Rgba4b colour);
    void DrawLeft(float x, float y, const std::string& text, const je::Rgba4b colour, je::Rgba4b shadowColour);

    void DrawCentred(float x, float y, const std::string& text, je::Rgba4b colour);
    void DrawCentred(float x, float y, const std::string& text, je::Rgba4b colour, je::Rgba4b shadowColour);

private:
    void Draw(float x, float y, const char* text, const je::Rgba4b colour);

    const je::TextureRegion& tiles_;
    je::Batch& batch_;
    float tileWidth_;
    float tileHeight_;
};
