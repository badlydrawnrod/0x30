#pragma once

#include "TextRenderer.h"

#include "je/Types.h"

class LevelRenderer
{

public:
    LevelRenderer(TextRenderer& textRenderer);

    void Draw(je::Vec2f position, int level);

private:
    TextRenderer textRenderer_;
};
