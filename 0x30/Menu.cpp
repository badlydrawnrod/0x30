#include "Menu.h"

#include "Constants.h"
#include "Input.h"

#include "je/Logger.h"
#include "je/QuadHelpers.h"
#include "je/Time.h"

#include <iomanip>
#include <sstream>


Menu::Menu(je::Batch& batch, Textures& textures) :
    batch_{ batch },
    textures_{ textures },
    textRenderer_{ textures.textTiles, batch },
    scores_{}
{
}


void Menu::Start(double t, int maxLevel, const std::array<uint64_t, 10>& scores)
{
    screenStartTime_ = t;
    maxLevel_ = maxLevel;
    scores_ = scores;

    // What are the scores for each level?
    for (auto i = 0; i < scores_.size(); i++)
    {
        LOG("Level: " << (i + 1) << " Score: " << scores_[i]);
    }
}


Screens Menu::Update(double t, double dt)
{
    if (input::buttons.JustPressed(input::ButtonId::a))
    {
        return Screens::Playing;
    }

    if (input::buttons.JustPressed(input::ButtonId::b))
    {
        return Screens::Quit;
    }

    if (input::buttons.JustPressed(input::ButtonId::up) && currentSelection_ > 0)
    {
        --currentSelection_;
    }

    if (input::buttons.JustPressed(input::ButtonId::down) && currentSelection_ + 1 < maxLevel_)
    {
        ++currentSelection_;
    }

    return Screens::Menu;
}


void Menu::Draw(double t)
{
    // Tell the player what to do.
    float x = VIRTUAL_WIDTH / 2.0f - 64.0f;
    float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f;
    textRenderer_.Draw(x + 1.0f, y + 1.0f, "PRESS [A] TO START", { 0x00, 0x00, 0x00, 0xff });
    textRenderer_.Draw(x, y, "PRESS [A] TO START");

    // Draw the game mode's name.
    x += 16.0f;
    y += 24.0f;
    textRenderer_.Draw(x + 1.0f, y + 1.0f, "Just a ninute", { 0x00, 0x00, 0x00, 0xff });
    textRenderer_.Draw(x, y, "Just a minute", { 0xff, 0x00, 0x00, 0xff });

    // Draw the level selection cursor.
    y += 16.0f;
    batch_.AddVertices(je::quads::Create(textures_.whiteSquare, x - 8.0f, y + 12.0f * currentSelection_ - 2.0f, 120.0f, 8.0f + 4.0f, { 0x00, 0x7f, 0x7f, 0xff }));

    // Draw the scores for each level.
    for (auto i = 0; i < scores_.size(); i++)
    {
        std::stringstream text;
        text << std::setw(2) << (i + 1) << std::setw(0) << "    " << std::setw(6) << scores_[i];
        textRenderer_.Draw(x + 1.0f, y + 1.0f, text.str(), { 0x00, 0x00, 0x00, 0xff });
        if (i + 1 <= maxLevel_)
        {
            textRenderer_.Draw(x, y, text.str());
        }
        else
        {
            textRenderer_.Draw(x, y, text.str(), { 0x7f, 0x7f, 0x7f, 0xff });
        }
        y += 12.0f;
    }
}
