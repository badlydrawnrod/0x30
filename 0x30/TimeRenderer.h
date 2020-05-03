#pragma once

#include "TextRenderer.h"

#include "je/Types.h"


class TimeRenderer
{
public:
    TimeRenderer(TextRenderer& textRenderer, const std::string& text);

    void Draw(je::Vec2f position, double elapsed);

private:
    TextRenderer& textRenderer_;
    std::string text_;
    int minutes_;
    int seconds_;
    char timeBuf_[128] = { 0 };
    int numChars_;
};
