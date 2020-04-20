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
        source_.Play(sounds_.menuSelect);
        return Screens::Menu;
    }

    return Screens::Dedication;
}


void Dedication::Draw(double t)
{
    // Draw some a title.
    float x = VIRTUAL_WIDTH / 2.0f - 5.5f * 8.0f;
    float y = 4.0f;
    textRenderer_.Draw(x + 1.0f, y + 1.0f, "FORTY EIGHT", { 0x7f, 0x00, 0x3f, 0xff });
    textRenderer_.Draw(x, y, "FORTY EIGHT", { 0xff, 0x00, 0x7f, 0xff });

    // And a dedication.
    x = VIRTUAL_WIDTH / 2.0f - 7 * 8.0f;
    y = VIRTUAL_HEIGHT / 3.0f;
    textRenderer_.Draw(x + 1.0f, y + 1.0f, "For Jessica...", { 0x00, 0x00, 0x00, 0xff });
    textRenderer_.Draw(x, y, "For Jessica...", { 0xff, 0x00, 0x7f, 0xff });
}
