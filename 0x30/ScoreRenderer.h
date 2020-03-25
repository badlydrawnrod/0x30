#pragma once

#include "TextRenderer.h"

#include "je/Types.h"


class ScoreRenderer
{
public:
    ScoreRenderer(TextRenderer& textRenderer);

    void Draw(je::Vec2f position, uint64_t score);

private:
    TextRenderer textRenderer_;
};
