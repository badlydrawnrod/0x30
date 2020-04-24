#include "Menu.h"

#include "Constants.h"
#include "Input.h"
#include "Types.h"

#include "je/Logger.h"
#include "je/QuadHelpers.h"
#include "je/Time.h"

#include <iomanip>
#include <sstream>


Menu::Menu(const Progress& progress, je::Batch& batch, Textures& textures) :
    progress_{ progress },
    batch_{ batch },
    textures_{ textures },
    textRenderer_{ textures.textTiles, batch }
{
}


void Menu::Start(double t)
{
    screenStartTime_ = t;

    // What are the scores for each level?
    const Scores& scores = progress_.LevelScores();
    for (auto i = 0; i < scores.size(); i++)
    {
        LOG("Level: " << (i + 1) << " Score: " << scores[i].score << " Player: " << scores[i].name);
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
        firstVisibleLevel_ = std::min(currentSelection_, firstVisibleLevel_);
    }

    if (input::buttons.JustPressed(input::ButtonId::down) && currentSelection_ + 1 < progress_.MaxLevel())
    {
        ++currentSelection_;
        if (currentSelection_ >= firstVisibleLevel_ + visibleLevels_)
        {
            ++firstVisibleLevel_;
        }
    }

    return Screens::Menu;
}


void Menu::Draw(double t)
{
    // Tell the player what to do.

    float x = VIRTUAL_WIDTH / 2.0f - (sizeof(TITLE) - 1) / 2.0f * 8.0f;
    float y = 4.0f;

    textRenderer_.Draw(x + 1.0f, y + 1.0f, TITLE, { 0x7f, 0x00, 0x3f, 0xff });
    textRenderer_.Draw(x, y, TITLE, { 0xff, 0x00, 0x7f, 0xff });
    y += 16.0f;

    x = 4.0f;
    textRenderer_.Draw(x + 1.0f, y + 1.0f, "Clear blocks and don't let them reach", { 0x00, 0x00, 0x00, 0xff });
    textRenderer_.Draw(x, y, "Clear blocks and don't let them reach");
    y += 12.0f;

    if (input::HasGamepad())
    {
        textRenderer_.Draw(x + 1.0f, y + 1.0f, "the top. Press (A) to swap and hold (X) to", { 0x00, 0x00, 0x00, 0xff });
        textRenderer_.Draw(x, y, "the top. Press ({) to swap and hold (}) to");
        y += 12.0f;
        textRenderer_.Draw(x + 1.0f, y + 1.0f, "raise the blocks more quickly.", { 0x00, 0x00, 0x00, 0xff });
        textRenderer_.Draw(x, y, "raise the blocks more quickly.");
    }
    else
    {
        textRenderer_.Draw(x + 1.0f, y + 1.0f, "the top. Press [SPACE] to swap blocks", { 0x00, 0x00, 0x00, 0xff });
        textRenderer_.Draw(x, y, "the top. Press [SPACE] to swap blocks");
        y += 12.0f;
        textRenderer_.Draw(x + 1.0f, y + 1.0f, "and hold [CTRL] to raise the blocks", { 0x00, 0x00, 0x00, 0xff });
        textRenderer_.Draw(x, y, "and hold [CTRL] to raise the blocks");
        y += 12.0f;
        textRenderer_.Draw(x + 1.0f, y + 1.0f, "more quickly.", { 0x00, 0x00, 0x00, 0xff });
        textRenderer_.Draw(x, y, "more quickly.");
    }

    y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 40.0f;
    if (std::fmod(t - screenStartTime_, 1.0f) < 0.6f)
    {
        if (input::HasGamepad())
        {
            x = VIRTUAL_WIDTH / 2.0f - 76.0f;
            textRenderer_.Draw(x + 1.0f, y + 1.0f, "PRESS (A) TO START", { 0x00, 0x00, 0x00, 0xff });
            textRenderer_.Draw(x, y, "PRESS ({) TO START", { 0xff, 0xff, 0xff, 0xff });
        }
        else
        {
            x = VIRTUAL_WIDTH / 2.0f - 22 * 0.5f * 8.0f;
            textRenderer_.Draw(x + 1.0f, y + 1.0f, "PRESS [SPACE] TO START", { 0x00, 0x00, 0x00, 0xff });
            textRenderer_.Draw(x, y, "PRESS [SPACE] TO START", { 0xff, 0xff, 0xff, 0xff });
        }
    }

    // Draw the game mode's name.
    x = VIRTUAL_WIDTH / 2.0f - 64.0f;
    y += 24.0f;
    textRenderer_.Draw(x + 1.0f, y + 1.0f, "\"Just a minute\"", { 0x00, 0x00, 0x00, 0xff });
    textRenderer_.Draw(x, y, "\"Just a minute\"", { 0xff, 0x00, 0x00, 0xff });
    x += 8.0f;

    // Draw the level selection cursor.
    y += 16.0f;
    int cursorRow = currentSelection_ - firstVisibleLevel_;
    batch_.AddVertices(je::quads::Create(textures_.whiteSquare, x - 8.0f, y + 12.0f * cursorRow - 2.0f, 120.0f, 8.0f + 4.0f, { 0x00, 0x7f, 0x7f, 0xff }));

    // TODO: add labels to make it clear that these are the best scores for each level, not a high score table.
    // Draw the scores for each level.
    const Scores& scores = progress_.LevelScores();
    const int maxLevel = progress_.MaxLevel();
    const int lastVisibleLevel = std::min(firstVisibleLevel_ + visibleLevels_, static_cast<int>(scores.size()));
    for (auto i = firstVisibleLevel_; i < lastVisibleLevel; i++)
    {
        std::stringstream text;
        text << std::setw(2) << (i + 1) << std::setw(0) << "    " << std::setw(6) << scores[i].score; // TODO: display the name.
        textRenderer_.Draw(x + 1.0f, y + 1.0f, text.str(), { 0x00, 0x00, 0x00, 0xff });
        if (i + 1 <= maxLevel)
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
