#include "Menu.h"

#include "Colours.h"
#include "Constants.h"
#include "Buttons.h"
#include "Types.h"

#include "je/Human.h"
#include "je/Logger.h"
#include "je/QuadHelpers.h"
#include "je/Time.h"

#include <cmath>
#include <iomanip>
#include <sstream>


Menu::Menu(Buttons& buttons, const Progress& progress, je::Batch& batch, Textures& textures) :
    buttons_{ buttons },
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
        LOG("Level: " << (i + 1) << " Score: " << scores[i].score);
    }
}


Screens Menu::Update(double /*t*/, double /*dt*/)
{
    if (buttons_.JustPressed(ButtonId::a))
    {
        return Screens::Playing;
    }

    if (buttons_.JustPressed(ButtonId::b))
    {
        return Screens::Quit;
    }

    if (buttons_.JustPressed(ButtonId::left) || buttons_.JustPressed(ButtonId::right))
    {
        if (mode_ == Mode::ENDLESS)
        {
            mode_ = Mode::TIMED;
            currentSelection_ = std::min(currentSelection_, progress_.MaxLevel() - 1);
        }
        else if (mode_ == Mode::TIMED)
        {
            mode_ = Mode::ENDLESS;
            currentSelection_ = std::min(currentSelection_, progress_.MaxTimedLevel() - 1);
        }
        firstVisibleLevel_ = 0;
    }

    if (buttons_.JustPressed(ButtonId::up) && currentSelection_ > 0)
    {
        --currentSelection_;
        firstVisibleLevel_ = std::min(currentSelection_, firstVisibleLevel_);
    }

    int maxSelection = 0;
    if (mode_ == Mode::TIMED)
    {
        maxSelection = progress_.MaxLevel();
    }
    else if (mode_ == Mode::ENDLESS)
    {
        maxSelection = progress_.MaxTimedLevel();
    }

    if (buttons_.JustPressed(ButtonId::down) && currentSelection_ + 1 < maxSelection)
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

    float x = VIRTUAL_WIDTH / 2.0f;
    float y = 4.0f;

    textRenderer_.DrawCentred(x, y, TITLE, Colours::title, Colours::titleShadow);
    y += 16.0f;

    x = 4.0f;
    textRenderer_.DrawLeft(x, y, "Clear blocks and don't let them reach", Colours::white);
    y += 12.0f;

    if (je::Human::Instance()->HasGamepad())
    {
        textRenderer_.DrawLeft(x, y, "the top. Press ({) to swap and hold (}) to", Colours::white);
        y += 12.0f;
        textRenderer_.DrawLeft(x, y, "raise the blocks more quickly.", Colours::white);
    }
    else
    {
        textRenderer_.DrawLeft(x, y, "the top. Press [SPACE] to swap blocks", Colours::white);
        y += 12.0f;
        textRenderer_.DrawLeft(x, y, "and hold [CTRL] to raise the blocks", Colours::white);
        y += 12.0f;
        textRenderer_.DrawLeft(x, y, "more quickly.", Colours::white);
    }

    y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 40.0f;
    if (std::fmod(t - screenStartTime_, 1.0f) < 0.6f)
    {
        if (je::Human::Instance()->HasGamepad())
        {
            x = VIRTUAL_WIDTH / 2.0f - 76.0f;
            textRenderer_.DrawLeft(x, y, "PRESS ({) TO START", Colours::white);
        }
        else
        {
            x = VIRTUAL_WIDTH / 2.0f - 22 * 0.5f * 8.0f;
            textRenderer_.DrawLeft(x, y, "PRESS [SPACE] TO START", Colours::white);
        }
    }

    // Draw the game mode's name.
    x = VIRTUAL_WIDTH / 2.0f;
    y += 24.0f;
    if (mode_ == Mode::TIMED)
    {
        textRenderer_.DrawCentred(x, y, "Just a minute", Colours::mode);
    }
    else if (mode_ == Mode::ENDLESS)
    {
        textRenderer_.DrawCentred(x, y, "Endless fun", Colours::mode);
    }
    x = VIRTUAL_WIDTH / 2.0f - 64.0f + 8.0f;

    // Draw the level selection cursor.
    y += 16.0f;
    int cursorRow = currentSelection_ - firstVisibleLevel_;
    batch_.AddVertices(je::quads::Create(textures_.whiteSquare, x - 8.0f, y + 12.0f * cursorRow - 2.0f, 120.0f, 8.0f + 4.0f, Colours::cursorBackground));
    if (mode_ == Mode::TIMED)
    {
        // Draw the scores for each level.
        const Scores& scores = progress_.LevelScores();
        const int maxLevel = progress_.MaxLevel();
        const int lastVisibleLevel = std::min(firstVisibleLevel_ + visibleLevels_, static_cast<int>(scores.size()));
        for (auto i = firstVisibleLevel_; i < lastVisibleLevel; i++)
        {
            std::stringstream text;
            text << std::setw(2) << (i + 1) << std::setw(0) << "    " << std::setw(6) << scores[i].score;
            if (i + 1 <= maxLevel)
            {
                textRenderer_.DrawLeft(x, y, text.str(), Colours::selectableLevel);
            }
            else
            {
                textRenderer_.DrawLeft(x, y, text.str(), Colours::unselectableLevel);
            }
            y += 12.0f;
        }
    }
    else if (mode_ == Mode::ENDLESS)
    {
        // Draw the times for each level.
        const Times& times = progress_.LevelTimes();
        const int maxLevel = progress_.MaxTimedLevel();
        const int lastVisibleLevel = std::min(firstVisibleLevel_ + visibleLevels_, static_cast<int>(times.size()));
        static const char* levels[] = {
            "Slow",
            "Medium",
            "Fast"
        };
        for (auto i = firstVisibleLevel_; i < lastVisibleLevel; i++)
        {
            double elapsed = times[i].time;
            int minutes = (int)(elapsed / 60.0);
            int seconds = ((int)elapsed % 60);

            std::stringstream text;
            text << std::setw(6) << levels[i] << std::setw(0) << "  " << std::setw(2) << minutes << "'" << std::setw(2) << std::setfill('0') << seconds << std::setfill(' ') << std::setw(0);
            if (i + 1 <= maxLevel)
            {
                textRenderer_.DrawLeft(x, y, text.str(), Colours::selectableLevel);
            }
            else
            {
                textRenderer_.DrawLeft(x, y, text.str(), Colours::unselectableLevel);
            }
            y += 12.0f;
        }
    }
}
