#include "Dedication.h"

#include "Constants.h"
#include "Input.h"

#include "je/Logger.h"
#include "je/Time.h"


Dedication::Dedication(je::Batch& batch, Textures& textures) :
    batch{ batch },
    textures{ textures },
    textRenderer{ textures.textTiles, batch },
    startTime_{ je::GetTime() }
{
}


Screens Dedication::Update()
{
    if (je::GetTime() >= startTime_ + 2.0)
    {
        return Screens::Menu;
    }

    if (input::wasSwapPressed && !input::swapPressed)
    {
        return Screens::Menu;
    }

    return Screens::Dedication;
}


void Dedication::Draw()
{
    // Draw some text.
    const float x = VIRTUAL_WIDTH / 2.0f - 7 * 8.0f;
    const float y = VIRTUAL_HEIGHT / 3.0f;
    textRenderer.Draw(x + 1.0f, y + 1.0f, "For Jessica...", { 0x00, 0x00, 0x00, 0xff });
    textRenderer.Draw(x, y, "For Jessica...", { 0xff, 0x00, 0x7f, 0xff });
}
