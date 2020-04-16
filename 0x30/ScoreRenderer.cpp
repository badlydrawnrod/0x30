#include "ScoreRenderer.h"

#include <iomanip>
#include <sstream>


ScoreRenderer::ScoreRenderer(TextRenderer& textRenderer, const std::string& label) : textRenderer_{ textRenderer }, label_{ label }
{
}


void ScoreRenderer::Draw(je::Vec2f position, uint64_t score)
{
    const je::Rgba4b blackColour{ 0x00, 0x00, 0x00, 0xff };
    const je::Rgba4b textColour{ 0x1f, 0xff, 0xff, 0xff };
    const je::Rgba4b scoreColour{ 0xff, 0x1f, 0x1f, 0xff };
    textRenderer_.Draw(1.0f + position.x, 1.0f + position.y, label_, blackColour);
    textRenderer_.Draw(position.x, position.y, label_, textColour);
    std::ostringstream scoreString;
    scoreString << std::setw(8) << score;
    textRenderer_.Draw(1.0f + position.x - 24.0f, 1.0f + position.y + 10.0f, scoreString.str(), blackColour);
    textRenderer_.Draw(position.x - 24.0f, position.y + 10.0f, scoreString.str(), scoreColour);
}
