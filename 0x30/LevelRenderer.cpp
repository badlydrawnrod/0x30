#include "LevelRenderer.h"

#include "Colours.h"
#include "TextRenderer.h"

#include <iomanip>
#include <sstream>

LevelRenderer::LevelRenderer(TextRenderer& textRenderer)
    : textRenderer_{textRenderer}
{
}

void LevelRenderer::Draw(je::Vec2f position, int level)
{
    textRenderer_.DrawLeft(position.x, position.y, "LEVEL", Colours::levelText, Colours::black);
    std::ostringstream levelString;
    levelString << std::setw(2) << level;

    textRenderer_.DrawLeft(position.x + 24.0f, position.y + 10.0f, levelString.str(), Colours::levelNumber, Colours::black);
}
