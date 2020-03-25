#include "TimeRenderer.h"

#include "je/Types.h"


TimeRenderer::TimeRenderer(TextRenderer& textRenderer) : textRenderer_{ textRenderer }, minutes_{ -1 }, seconds_{ -1 }, numChars_{ 0 }
{
}


void TimeRenderer::Draw(je::Vec2f position, double elapsed)
{
    int minutes = (int)(elapsed / 60.0);
    int seconds = ((int)elapsed % 60);

    // Only re-create the string if it has changed.
    if (minutes != minutes_ || seconds != seconds_)
    {
        numChars_ = sprintf_s(timeBuf_, sizeof(timeBuf_), "%d'%02d", minutes, seconds);
        minutes_ = minutes;
        seconds_ = seconds;
    }

    // TODO: juice it up.
    // Draw "TIME" on one row, with the elapsed time on the following row, right-justified to "TIME".
    je::Rgba4b blackColour{ 0x00, 0x00, 0x00, 0xff };
    je::Rgba4b textColour{ 0x1f, 0xff, 0xff, 0xff };
    je::Rgba4b timeColour{ 0xff, 0x1f, 0x1f, 0xff };
    textRenderer_.Draw(1.0f + position.x, 1.0f + position.y, "TIME", blackColour);
    textRenderer_.Draw(position.x, position.y, "TIME", textColour);
    // TODO: lose the magic numbers.
    textRenderer_.Draw(1.0f + position.x + 32.0f - 8.0f * numChars_, 1.0f + position.y + 10.0f, timeBuf_, blackColour);
    textRenderer_.Draw(position.x + 32.0f - 8.0f * numChars_, position.y + 10.0f, timeBuf_, timeColour);
}
