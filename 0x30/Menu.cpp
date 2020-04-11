#include "Menu.h"

#include "Constants.h"
#include "Input.h"

#include "je/Logger.h"
#include "je/Time.h"


Menu::Menu(je::Batch& batch, Textures& textures) :
    batch_{ batch },
    textures_{ textures },
    textRenderer_{ textures.textTiles, batch }
{
}


Screens Menu::Update(double t, double dt)
{
    if (input::WasAPressed() && !input::IsAPressed())
    {
        return Screens::Playing;
    }
    
    if (input::WasBPressed() && !input::IsBPressed())
    {
        return Screens::Quit;
    }

    return Screens::Menu;
}


void Menu::Draw()
{
    // Draw some text.
    const float x = VIRTUAL_WIDTH / 2.0f - 40.0f;
    const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f;
    textRenderer_.Draw(x + 1.0f, y + 1.0f, "PRESS START", { 0x00, 0x00, 0x00, 0xff });
    textRenderer_.Draw(x, y, "PRESS START");
}
