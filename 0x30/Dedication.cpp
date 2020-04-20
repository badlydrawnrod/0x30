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
    if (!sounds_.IsLoaded())
    {
        return Screens::Dedication;
    }

    return Screens::Menu;
}


void Dedication::Draw(double t)
{
    // Draw some a title.
    float x = VIRTUAL_WIDTH / 2.0f - 5.5f * 8.0f;
    float y = 4.0f;
    textRenderer_.Draw(x + 1.0f, y + 1.0f, "FORTY EIGHT", { 0x7f, 0x00, 0x3f, 0xff });
    textRenderer_.Draw(x, y, "FORTY EIGHT", { 0xff, 0x00, 0x7f, 0xff });

    // And a dedication.
    if (t - startTime_ >= 1.0)
    {
        x = VIRTUAL_WIDTH / 2.0f - 3 * 8.0f;
        y = VIRTUAL_HEIGHT / 3.0f - 24.0f;
        textRenderer_.Draw(x + 1.0f, y + 1.0f, "A game", { 0x00, 0x00, 0x00, 0xff });
        textRenderer_.Draw(x, y, "A game", { 0xff, 0x00, 0x7f, 0xff });
    }

    if (t - startTime_ >= 3.0)
    {
        x = VIRTUAL_WIDTH / 2.0f - 7 * 8.0f;
        y = VIRTUAL_HEIGHT / 3.0f - 12.0f;
        textRenderer_.Draw(x + 1.0f, y + 1.0f, "made with love", { 0x00, 0x00, 0x00, 0xff });
        textRenderer_.Draw(x, y, "made with love", { 0xff, 0x00, 0x7f, 0xff });
    }

    if (t - startTime_ >= 5.0)
    {
        x = VIRTUAL_WIDTH / 2.0f - 5.5 * 8.0f;
        y = VIRTUAL_HEIGHT / 3.0f;
        textRenderer_.Draw(x + 1.0f, y + 1.0f, "for Jessica", { 0x00, 0x00, 0x00, 0xff });
        textRenderer_.Draw(x, y, "for Jessica", { 0xff, 0x00, 0x7f, 0xff });
    }

    // Tell the player that it's loading.
    if (t - startTime_ >= 8.0 && std::fmod(t - startTime_, 1.0) < 0.6)
    {
        x = VIRTUAL_WIDTH / 2.0f - 3.5 * 8.0f;
        y = 3 * VIRTUAL_HEIGHT / 4.0f;
        textRenderer_.Draw(x + 1.0f, y + 1.0f, "Loading", { 0x00, 0x00, 0x00, 0xff });
        textRenderer_.Draw(x, y, "Loading", { 0xff, 0xff, 0x00, 0xff });
    }
}
