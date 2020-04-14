#include "Dedication.h"

#include "Constants.h"
#include "Input.h"
#include "Types.h"

#include "je/Logger.h"
#include "je/Time.h"


Dedication::Dedication(je::Batch& batch, Textures& textures, Sounds& sounds) :
    batch_{ batch },
    textures_{ textures },
    sounds_{ sounds },
    textRenderer_{ textures.textTiles, batch },
    startTime_{ je::GetTime() }
{
}


Screens Dedication::Update(double t, double dt)
{
    if (t >= startTime_ + 2.0)
    {
        return Screens::Menu;
    }

    if (input::buttons.JustPressed(input::ButtonId::a))
    {
        je::Play(sounds_.menuSelect, source_);
        return Screens::Menu;
    }

    return Screens::Dedication;
}


void Dedication::Draw(double t)
{
    // Draw some text.
    const float x = VIRTUAL_WIDTH / 2.0f - 7 * 8.0f;
    const float y = VIRTUAL_HEIGHT / 3.0f;
    textRenderer_.Draw(x + 1.0f, y + 1.0f, "For Jessica...", { 0x00, 0x00, 0x00, 0xff });
    textRenderer_.Draw(x, y, "For Jessica...", { 0xff, 0x00, 0x7f, 0xff });
}
