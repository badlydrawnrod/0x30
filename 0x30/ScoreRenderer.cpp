#include "ScoreRenderer.h"

#include "Colours.h"

#include <iomanip>
#include <sstream>

ScoreRenderer::ScoreRenderer(TextRenderer& textRenderer, const std::string& label)
    : textRenderer_{textRenderer}, label_{label}
{
}

void ScoreRenderer::Draw(je::Vec2f position, uint64_t score)
{
    textRenderer_.DrawLeft(position.x, position.y, label_, Colours::scoreText, Colours::black);
    std::ostringstream scoreString;
    scoreString << std::setw(8) << score;
    textRenderer_.DrawLeft(position.x - 24.0f, position.y + 10.0f, scoreString.str(), Colours::scoreNumber, Colours::black);
}
