#include "LevelRenderer.h"
#include "TextRenderer.h"

#include <iomanip>
#include <sstream>


LevelRenderer::LevelRenderer(TextRenderer& textRenderer) : textRenderer_{ textRenderer }
{
}

void LevelRenderer::Draw(je::Vec2f position, int level)
{
    je::Rgba4b blackColour{ 0x00, 0x00, 0x00, 0xff };
    je::Rgba4b textColour{ 0x1f, 0xff, 0xff, 0xff };
    je::Rgba4b levelColour{ 0xff, 0x1f, 0x1f, 0xff };
    textRenderer_.Draw(1.0f + position.x, 1.0f + position.y, "LEVEL", blackColour);
    textRenderer_.Draw(position.x, position.y, "LEVEL", textColour);
    std::ostringstream levelString;
    levelString << std::setw(2) << level;

    textRenderer_.Draw(1.0f + position.x + 24.0f, 1.0f + position.y + 10.0f, levelString.str(), blackColour);
    textRenderer_.Draw(position.x + 24.0f, position.y + 10.0f, levelString.str(), levelColour);
}
