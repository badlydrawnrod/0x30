#include "SpeedRenderer.h"
#include "TextRenderer.h"

#include <iomanip>
#include <sstream>


SpeedRenderer::SpeedRenderer(TextRenderer& textRenderer) : textRenderer_{ textRenderer }
{
}

void SpeedRenderer::Draw(je::Vec2f position, int speed)
{
    je::Rgba4b blackColour{ 0x00, 0x00, 0x00, 0xff };
    je::Rgba4b textColour{ 0x1f, 0xff, 0xff, 0xff };
    je::Rgba4b speedColour{ 0xff, 0x1f, 0x1f, 0xff };
    textRenderer_.Draw(1.0f + position.x, 1.0f + position.y, "SPEED", blackColour);
    textRenderer_.Draw(position.x, position.y, "SPEED", textColour);
    std::ostringstream speedString;
    speedString << std::setw(1) << speed;

    textRenderer_.Draw(1.0f + position.x + 32.0f, 1.0f + position.y + 10.0f, speedString.str(), blackColour);
    textRenderer_.Draw(position.x + 32.0f, position.y + 10.0f, speedString.str(), speedColour);
}
