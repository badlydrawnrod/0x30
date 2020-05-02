#include "Playing.h"

#include "Colours.h"
#include "Input.h"
#include "Types.h"

#include "je/Logger.h"

#include <algorithm>
#include <cmath>


Playing::Playing(Progress& progress, je::Batch& batch, Textures& textures, Sounds& sounds, std::function<int(int, int)>& rnd) :
    batch_{ batch },
    textures_{ textures },
    progress_{ progress },
    sounds_{ sounds },
    pit_{ rnd },
    pitRenderer_{ pit_, textures, batch },
    textRenderer_{ textures.textTiles, batch },
    timeRenderer_{ textRenderer_ },
    scoreRenderer_{ textRenderer_, "SCORE" },
    highScoreRenderer_{ textRenderer_, " HIGH" },
    speedRenderer_{ textRenderer_ },
    flyupRenderer_{ textures, batch_ },
    state_{ State::PLAYING }
{
}


void Playing::SetLevel(int level)
{
    level_ = level;
    progress_.UpdateMaxLevel(level_);
}


void Playing::SetState(State state, double t)
{
    state_ = state;
    if (state_ == State::GAME_OVER)
    {
        progress_.UpdateHighScore(level_, highScore_);
    }
    stateStartTime_ = t;
}


void Playing::Start(const double t, const int level)
{
    int actualLevel = level < numLevels_ ? level : numLevels_;
    SetLevel(actualLevel);
    highScore_ = progress_.HighScore(level_);
    pit_.Reset(actualLevel);
    SetState(State::PLAYING, t);
    score_ = 0;
    remainingTime_ = 98.0;  // Enough time to play the minute waltz. Because it's my newt not minute.
    elapsedTime_ = 0.0;
    lastTime_ = t;
    lastPlayed_ = actualLevel;

    // The scroll rate is based on the level number, but doesn't increase when new blocks are introduced.
    int speedMultiplier = actualLevel - 1;
    if (actualLevel >= 16)
    {
        --speedMultiplier;
    }
    if (actualLevel >= 9)
    {
        --speedMultiplier;
    }
    if (actualLevel >= 4)
    {
        --speedMultiplier;
    }
    LOG("Level " << actualLevel << ", speed " << speedMultiplier);
    scrollRate_ = 0.025f + (0.0025f * speedMultiplier);

    cursorTileX_ = (Pit::cols / 2) - 1;
    cursorTileY_ = Pit::rows / 2;
    flyupRenderer_.Reset();
    musicSource_.Play(sounds_.musicMinuteWaltz);
}


void Playing::UpdateScore()
{
    const auto& runs = pit_.Runs();
    if (runs.size() > 0)
    {
        auto multiplier = runs.size();
        LOG("Run info");
        int n = 1;
        for (auto runInfo : runs)
        {
            LOG(n << " size: " << runInfo.runSize << ", chain: " << runInfo.chainLength);
            ++n;
            int runScore = 0;
            switch (runInfo.runSize)
            {
            case 3:
                runScore = 10;
                break;
            case 4:
                runScore = 25;
                break;
            case 5:
                runScore = 50;
                break;
            case 6:
                runScore = 100;
                break;
            case 7:
                runScore = 250;
                break;
            case 8:
                runScore = 500;
                break;
            case 9:
                runScore = 1000;
                break;
            default:
                break;
            }
            uint64_t scoreChange = runScore * (runInfo.chainLength + 1) * multiplier;
            LOG("Run score: " << runScore << " * chain length " << (runInfo.chainLength + 1) << " * multiplier " << multiplier << " = " << scoreChange);
            score_ += scoreChange;
        }
        highScore_ = std::max(score_, highScore_);
        LOG("Score: " << score_ << " High: " << highScore_);
    }
}


void Playing::UpdatePlaying(double t)
{
    // Scroll the contents of the pit up.
    internalTileScroll_ += (input::buttons.IsPressed(input::ButtonId::x) && input::buttons.LastPressed(input::ButtonId::x) > stateStartTime_)
        ? 1.0f
        : scrollRate_;
    if (internalTileScroll_ >= tileSize_)
    {
        pit_.ScrollOne();
        if (cursorTileY_ > 1)
        {
            cursorTileY_--;
        }
        internalTileScroll_ = 0.0f;
    }

    // Move the player.
    if (input::buttons.JustPressed(input::ButtonId::left) && cursorTileX_ > 0)
    {
        --cursorTileX_;
    }
    if (input::buttons.JustPressed(input::ButtonId::right) && cursorTileX_ < Pit::cols - 2)
    {
        ++cursorTileX_;
    }
    if (input::buttons.JustPressed(input::ButtonId::up) && cursorTileY_ > 1)
    {
        --cursorTileY_;
    }
    if (input::buttons.JustPressed(input::ButtonId::down) && cursorTileY_ < Pit::rows - 2)
    {
        ++cursorTileY_;
    }

    // Swap tiles.
    if (input::buttons.JustPressed(input::ButtonId::a))
    {
        pit_.Swap(cursorTileX_, cursorTileY_);
        blocksSwappingSource_.Play(sounds_.blocksSwapping);
    }

    pit_.Update();

    if (pit_.Landed())
    {
        blocksLandingSource_.Play(sounds_.blocksLanding);
    }
    if (auto runs = pit_.Runs(); runs.size() > 0)
    {
        blocksPoppingSource_.Play(sounds_.blocksPopping);
    }

    UpdateScore();

    // Add fly-ups.
    for (const auto& run : pit_.Runs())
    {
        flyupRenderer_.AddFlyupsForRun(run, topLeft_, tileSize_, internalTileScroll_);
        flyupRenderer_.AddFlyupsForChains(run, topLeft_, tileSize_, internalTileScroll_);
    }

    // Check for paused.
    if (input::buttons.JustPressed(input::ButtonId::back))
    {
        musicSource_.Pause();
        SetState(State::PAUSED, t);
    }

    // Check for game over.
    if (pit_.IsImpacted())
    {
        musicSource_.Play(sounds_.musicLAdieu);
        SetState(State::GAME_OVER, t);
        actionsEnabled_ = false;
    }

}


Screens Playing::UpdateGameOver(double t)
{
    // Don't allow the player to do anything for a short time to prevent them from taking accidental actions.
    const double delay = 2.0;
    actionsEnabled_ = t - stateStartTime_ >= delay;
    if (actionsEnabled_)
    {
        if (input::buttons.JustPressed(input::ButtonId::b))
        {
            musicSource_.Stop();
            progress_.SaveScores(); // TODO: dedup.
            return Screens::Menu;
        }
        if (input::buttons.JustPressed(input::ButtonId::x))
        {
            musicSource_.Stop();
            progress_.SaveScores(); // TODO: dedup.
            Start(t, lastPlayed_);
        }
        if (input::buttons.JustPressed(input::ButtonId::a) && !pit_.IsImpacted())
        {
            // Go on to the next level.
            musicSource_.Stop();
            progress_.SaveScores(); // TODO: dedup.
            Start(t, level_);
        }
    }
    return Screens::Playing;
}


Screens Playing::UpdatePaused(double t)
{
    if (input::buttons.JustPressed(input::ButtonId::b))
    {
        musicSource_.Stop();
        return Screens::Menu;
    }
    if (input::buttons.JustPressed(input::ButtonId::a))
    {
        musicSource_.Resume();
        SetState(State::PLAYING, t);
    }
    return Screens::Playing;
}


Screens Playing::Update(double t, double /*dt*/)
{
    // Update elapsed time only when playing.
    double multiplier = (state_ == State::PLAYING) ? 1.0 : 0.0;
    double now = t;
    double delta = (now - lastTime_) * multiplier;
    lastTime_ = now;
    elapsedTime_ += delta;
    remainingTime_ -= delta;
    if (remainingTime_ < 0.0)
    {
        if (state_ == State::PLAYING)
        {
            musicSource_.Play(sounds_.musicHallelujah);
            SetState(State::GAME_OVER, t);
            SetLevel(level_ + 1);
            actionsEnabled_ = false;
        }
        remainingTime_ = 0.0;
    }

    if (state_ == State::PLAYING)
    {
        UpdatePlaying(t);
    }
    else if (state_ == State::GAME_OVER)
    {
        Screens screen = UpdateGameOver(t);
        if (screen != Screens::Playing)
        {
            return screen;
        }
    }
    else if (state_ == State::PAUSED)
    {
        Screens screen = UpdatePaused(t);
        if (screen != Screens::Playing)
        {
            return screen;
        }
    }

    // Remove dead fly-ups.
    flyupRenderer_.Update();

    return Screens::Playing;
}


void Playing::DrawBackdrop()
{
    if (!textures_.backdrops.empty())
    {
        int index = lastPlayed_ - 1;
        batch_.AddVertices(je::quads::Create(textures_.backdrops[index % textures_.backdrops.size()], 0.0f, 0.0f));
    }
}


void Playing::DrawPaused()
{
    // Draw a translucent texture over the pit area again.
    batch_.AddVertices(je::quads::Create(textures_.blankSquare, topLeft_.x, topLeft_.y, tileSize_ * pit_.cols, tileSize_ * (pit_.rows - 1)));

    // The game is currently paused.
    {
        const float x = VIRTUAL_WIDTH / 2.0f - 3 * 8.0f;
        const float y = VIRTUAL_HEIGHT / 3.0f;
        textRenderer_.DrawLeft(x, y, "Paused", Colours::white, Colours::black);
    }

    if (input::HasGamepad())
    {
        const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 8.0f;
        const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
        textRenderer_.DrawLeft(x, y, "(|) quit", Colours::white, Colours::black);
    }
    else
    {
        const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 8.0f;
        const float x = VIRTUAL_WIDTH / 2.0f - 6.125f * 8.0f;
        textRenderer_.DrawLeft(x, y, "[ESC]   quit", Colours::white, Colours::black);
    }

    if (input::HasGamepad())
    {
        const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 6.0f;
        const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
        textRenderer_.DrawLeft(x, y, "({) play", Colours::white, Colours::black);
    }
    else
    {
        const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 6.0f;
        const float x = VIRTUAL_WIDTH / 2.0f - 6.125f * 8.0f;
        textRenderer_.DrawLeft(x, y, "[SPACE] play", Colours::white, Colours::black);
    }
}


void Playing::DrawGameOver(double t)
{
    // Draw a translucent texture over the pit area again.
    batch_.AddVertices(je::quads::Create(textures_.blankSquare, topLeft_.x, topLeft_.y, tileSize_ * pit_.cols, tileSize_ * (pit_.rows - 1)));

    // It's game over, so tell the player.
    if (std::fmod(t - stateStartTime_, 1.0) < 0.6)
    {
        const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f;
        if (pit_.IsImpacted())
        {
            const float x = VIRTUAL_WIDTH / 2.0f - 5.0 * 8.0f;
            textRenderer_.DrawLeft(x, y, "GAME OVER!", Colours::white, Colours::black);
        }
        else
        {
            const float x = VIRTUAL_WIDTH / 2.0f - 4.0 * 8.0f;
            textRenderer_.DrawLeft(x, y, "YOU WIN!", Colours::white, Colours::black);
        }
    }
    if (actionsEnabled_)
    {
        {
            const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 4.0f;
            if (input::HasGamepad())
            {
                const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
                textRenderer_.DrawLeft(x, y, "(}) retry", Colours::white, Colours::black);
            }
            else
            {
                const float x = VIRTUAL_WIDTH / 2.0f - 6.125f * 8.0f;
                textRenderer_.DrawLeft(x, y, "[CTRL] retry", Colours::white, Colours::black);
            }
        }
        if (input::HasGamepad())
        {
            const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 8.0f;
            const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
            textRenderer_.DrawLeft(x, y, "(|) back", Colours::white, Colours::black);
        }
        else
        {
            const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 8.0f;
            const float x = VIRTUAL_WIDTH / 2.0f - 6.125f * 8.0f;
            textRenderer_.DrawLeft(x, y, "[ESC]   back", Colours::white, Colours::black);
        }
        if (!pit_.IsImpacted())
        {
            if (input::HasGamepad())
            {
                const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 6.0f;
                const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
                textRenderer_.DrawLeft(x, y, "({) next", Colours::white, Colours::black);
            }
            else
            {
                const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 6.0f;
                const float x = VIRTUAL_WIDTH / 2.0f - 6.125f * 8.0f;
                textRenderer_.DrawLeft(x, y, "[SPACE] next", Colours::white, Colours::black);
            }
        }
    }
}


void Playing::DrawTitle()
{
    const float x = VIRTUAL_WIDTH / 2;
    const float y = 4.0f;
    batch_.AddVertices(je::quads::Create(textures_.blankSquare, 0.0f, 2.0f, VIRTUAL_WIDTH, 12.0f));
    textRenderer_.DrawCentred(x, y, "Just a minute", Colours::mode, Colours::black);
}


void Playing::DrawStats()
{
    // Draw some stats.
    batch_.AddVertices(je::quads::Create(textures_.blankSquare, topLeft_.x - tileSize_ * 3 - tileSize_ * 0.5f, topLeft_.y + tileSize_ * 2 - tileSize_ * 0.5f, tileSize_ * 3, tileSize_ * 2));
    batch_.AddVertices(je::quads::Create(textures_.blankSquare, topLeft_.x + tileSize_ * (pit_.cols + 1) - tileSize_ * 0.5f, topLeft_.y + tileSize_ * 2 - tileSize_ * 0.5f, tileSize_ * 5, tileSize_ * 6));
    timeRenderer_.Draw({ topLeft_.x - tileSize_ * 3, topLeft_.y + tileSize_ * 2 }, remainingTime_);
    scoreRenderer_.Draw({ topLeft_.x + tileSize_ * (pit_.cols + 2.5f), topLeft_.y + tileSize_ * 2 }, score_);
    highScoreRenderer_.Draw({ topLeft_.x + tileSize_ * (pit_.cols + 2.5f), topLeft_.y + tileSize_ * 4 }, highScore_);
    speedRenderer_.Draw({ topLeft_.x + tileSize_ * (pit_.cols + 2.5f), topLeft_.y + tileSize_ * 6 }, lastPlayed_);
}


void Playing::DrawGui()
{
    DrawTitle();
    DrawStats();
}


void Playing::DrawPit()
{
    // Draw a translucent texture over the pit area, then draw the pit itself.
    batch_.AddVertices(je::quads::Create(textures_.blankSquare, topLeft_.x, topLeft_.y, tileSize_ * pit_.cols, tileSize_ * (pit_.rows - 1)));
    pitRenderer_.Draw(topLeft_, internalTileScroll_, bottomRow_);
}


void Playing::DrawCursor()
{
    // We're still playing, so draw the cursor.
    float cursorX = topLeft_.x + cursorTileX_ * tileSize_ - 1.0f;
    float cursorY = topLeft_.y + cursorTileY_ * tileSize_ - 1.0f - internalTileScroll_;
    batch_.AddVertices(je::quads::Create(textures_.cursorTile, cursorX, cursorY));
    batch_.AddVertices(je::quads::Create(textures_.cursorTile, cursorX + tileSize_, cursorY));
}


void Playing::Draw(double t)
{
    DrawBackdrop();
    DrawGui();
    DrawPit();

    if (state_ == State::PLAYING)
    {
        DrawCursor();
    }
    else if (state_ == State::GAME_OVER)
    {
        DrawGameOver(t);
    }
    else if (state_ == State::PAUSED)
    {
        DrawPaused();
    }

    flyupRenderer_.DrawFlyups();
}
