#pragma once

#include "TextRenderer.h"

#include "je/Types.h"


class ScoreRenderer
{
public:
    ScoreRenderer(TextRenderer& textRenderer, const std::string& label);

    void Draw(je::Vec2f position, uint64_t score);

private:
    TextRenderer textRenderer_;
    std::string label_;
};
