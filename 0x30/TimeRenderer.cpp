#include "TimeRenderer.h"

#include "Colours.h"

#include "je/Types.h"

#include <stdio.h>


TimeRenderer::TimeRenderer(TextRenderer& textRenderer, const std::string& text) : textRenderer_{ textRenderer }, text_{ text }, minutes_{ -1 }, seconds_{ -1 }, numChars_{ 0 }
{
}


void TimeRenderer::Draw(je::Vec2f position, double elapsed)
{
    int minutes = (int)(elapsed / 60.0);
    int seconds = ((int)elapsed % 60);

    // Only re-create the string if it has changed.
    if (minutes != minutes_ || seconds != seconds_)
    {
#if !defined(__EMSCRIPTEN__)
        numChars_ = sprintf_s(timeBuf_, sizeof(timeBuf_), "%d'%02d", minutes, seconds);
#else
        numChars_ = sprintf(timeBuf_, "%d'%02d", minutes, seconds);
#endif
        minutes_ = minutes;
        seconds_ = seconds;
    }

    // TODO: juice it up.
    // Draw "TIME" on one row, with the elapsed time on the following row, right-justified to "TIME".
    textRenderer_.DrawLeft(position.x, position.y, text_.c_str(), Colours::timeText, Colours::black);
    // TODO: lose the magic numbers.
    textRenderer_.DrawLeft(position.x + 32.0f - 8.0f * numChars_, position.y + 10.0f, timeBuf_, Colours::timeNumber, Colours::black);
}
