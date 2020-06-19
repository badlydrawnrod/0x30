#pragma once

#include "TextRenderer.h"

#include "je/Types.h"

class LevelRenderer
{

public:
    LevelRenderer(TextRenderer& textRenderer);

    void Draw(je::Vec2f position, size_t level);

private:
    TextRenderer textRenderer_;
};
