#pragma once

#include "TextRenderer.h"

#include "je/Types.h"


class SpeedRenderer
{

public:
    SpeedRenderer(TextRenderer& textRenderer);

    void Draw(je::Vec2f position);

private:
    TextRenderer textRenderer_;
};
