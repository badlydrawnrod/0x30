#include "Dedication.h"

#include "Colours.h"
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


Screens Dedication::Update(double t, double /*dt*/)
{
    if (!sounds_.IsLoaded())
    {
        return Screens::Dedication;
    }

    if (t - startTime_ >= 30.0 || input::buttons.JustPressed(input::ButtonId::a))
    {
        return Screens::Menu;
    }

    return Screens::Dedication;
}


void Dedication::Draw(double t)
{
    // Draw some a title.
    float x = VIRTUAL_WIDTH / 2.0f;
    float y = 4.0f;
    textRenderer_.DrawCentred(x, y, TITLE, Colours::title, Colours::titleShadow);

    // And a dedication.
    if (t - startTime_ >= 1.0)
    {
        x = VIRTUAL_WIDTH / 2.0f;
        y = VIRTUAL_HEIGHT / 3.0f - 24.0f;
        textRenderer_.DrawCentred(x, y, "A game", Colours::title);
    }

    if (t - startTime_ >= 2.0)
    {
        x = VIRTUAL_WIDTH / 2.0f;
        y = VIRTUAL_HEIGHT / 3.0f - 12.0f;
        textRenderer_.DrawCentred(x, y, "made with love", Colours::title);
    }

    if (t - startTime_ >= 3.0)
    {
        x = VIRTUAL_WIDTH / 2.0f;
        y = VIRTUAL_HEIGHT / 3.0f;
        textRenderer_.DrawCentred(x, y, "for Jessica", Colours::title);
    }

    // Tell the player that they can take an action.
    if (t - startTime_ >= 4.0 && sounds_.IsLoaded() && std::fmod(t - startTime_, 1.0) <= 0.6)
    {
        if (input::HasGamepad())
        {
            x = VIRTUAL_WIDTH / 2.0f;
            y = 3 * VIRTUAL_HEIGHT / 4.0f;
            textRenderer_.DrawCentred(x, y, "Press ({)", Colours::white);
        }
        else
        {
            x = VIRTUAL_WIDTH / 2.0f;
            y = 3 * VIRTUAL_HEIGHT / 4.0f;
            textRenderer_.DrawCentred(x, y, "Press [SPACE]", Colours::white);
        }
    }
}
