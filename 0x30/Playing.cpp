#include "Playing.h"

#include "Colours.h"
#include "Input.h"
#include "Types.h"

#include "je/Logger.h"

#include <algorithm>
#include <cmath>


Playing::Playing(Progress& progress, je::Batch& batch, Textures& textures, Sounds& sounds, std::function<int(int, int)>& rnd) :
    batch_{ batch },
    textures{ textures },
    progress_{ progress },
    sounds_{ sounds },
    pit{ rnd },
    pitRenderer{ pit, textures, batch },
    textRenderer{ textures.textTiles, batch },
    timeRenderer{ textRenderer },
    scoreRenderer{ textRenderer, "SCORE" },
    highScoreRenderer{ textRenderer, " HIGH" },
    speedRenderer{ textRenderer },
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


void Playing::Start(const double t)
{
    Start(t, level_);
}


void Playing::Start(const double t, const int level)
{
    int actualLevel = level < numLevels ? level : numLevels;
    SetLevel(actualLevel);
    highScore_ = progress_.HighScore(level_);
    pit.Reset(actualLevel);
    SetState(State::PLAYING, t);
    score = 0;
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
    scrollRate = 0.025f + (0.0025f * speedMultiplier);

    cursorTileX = (Pit::cols / 2) - 1;
    cursorTileY = Pit::rows / 2;
    flyups.clear();
    musicSource_.Play(sounds_.musicMinuteWaltz);
}


void Playing::AddFlyupsForRun(const Pit::RunInfo& run)
{
    // Add fly-ups for runs of 4-9.
    if (run.runSize >= 4 && run.runSize < 10)
    {
        float runFlyupDuration = 1.0f;
        je::TextureRegion texture{};

        switch (run.runSize)
        {
        case 4:
            texture = textures.combo4;
            break;
        case 5:
            texture = textures.combo5;
            break;
        case 6:
            texture = textures.combo6;
            break;
        case 7:
            texture = textures.combo7;
            break;
        case 8:
            texture = textures.combo8;
            break;
        case 9:
            texture = textures.combo9;
            break;
        }

        if (run.runSize >= 4 && run.runSize <= 9)
        {
            for (auto i = 0; i < run.runSize; i++)
            {
                float x = run.coord[i].x * tileSize + topLeft.x + tileSize * 0.5f - texture.w * 0.5f;
                float y = run.coord[i].y * tileSize + topLeft.y + tileSize * 0.5f - texture.h * 0.5f - internalTileScroll;
                flyups.emplace_back(texture, x, y, runFlyupDuration);
            }
        }
    }
}


void Playing::AddFlyupsForChains(const Pit::RunInfo& run)
{
    // Add fly-ups for chains of 2-6.
    if (const auto chains = run.chainLength + 1; chains >= 2 && chains < 7)
    {
        const float chainFlyupDuration = 1.5f;
        je::TextureRegion texture{};
        switch (chains)
        {
        case 2:
            texture = textures.chain2;
            break;
        case 3:
            texture = textures.chain3;
            break;
        case 4:
            texture = textures.chain4;
            break;
        case 5:
            texture = textures.chain5;
            break;
        case 6:
            texture = textures.chain6;
            break;
        }
        if (chains >= 2 && chains <= 6)
        {
            for (auto i = 0; i < run.runSize; i++)
            {
                float x = run.coord[i].x * tileSize + topLeft.x + tileSize * 0.5f - texture.w * 0.5f;
                float y = run.coord[i].y * tileSize + topLeft.y + tileSize * 0.5f - texture.h * 0.5f - internalTileScroll - tileSize;
                flyups.emplace_back(texture, x, y, chainFlyupDuration);
            }
        }
    }
}


void Playing::UpdateScore()
{
    const auto& runs = pit.Runs();
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
            score += scoreChange;
        }
        highScore_ = std::max(score, highScore_);
        LOG("Score: " << score << " High: " << highScore_);
    }
}


void Playing::UpdatePlaying(double t)
{
    // Scroll the contents of the pit up.
    internalTileScroll += (input::buttons.IsPressed(input::ButtonId::x) && input::buttons.LastPressed(input::ButtonId::x) > stateStartTime_)
        ? 1.0f
        : scrollRate;
    if (internalTileScroll >= tileSize)
    {
        pit.ScrollOne();
        if (cursorTileY > 1)
        {
            cursorTileY--;
        }
        internalTileScroll = 0.0f;
    }

    // Move the player.
    if (input::buttons.JustPressed(input::ButtonId::left) && cursorTileX > 0)
    {
        --cursorTileX;
    }
    if (input::buttons.JustPressed(input::ButtonId::right) && cursorTileX < Pit::cols - 2)
    {
        ++cursorTileX;
    }
    if (input::buttons.JustPressed(input::ButtonId::up) && cursorTileY > 1)
    {
        --cursorTileY;
    }
    if (input::buttons.JustPressed(input::ButtonId::down) && cursorTileY < Pit::rows - 2)
    {
        ++cursorTileY;
    }

    // Swap tiles.
    if (input::buttons.JustPressed(input::ButtonId::a))
    {
        pit.Swap(cursorTileX, cursorTileY);
        blocksSwappingSource_.Play(sounds_.blocksSwapping);
    }

    pit.Update();

    if (pit.Landed())
    {
        blocksLandingSource_.Play(sounds_.blocksLanding);
    }
    if (auto runs = pit.Runs(); runs.size() > 0)
    {
        blocksPoppingSource_.Play(sounds_.blocksPopping);
    }

    UpdateScore();

    // Add fly-ups.
    for (const auto& run : pit.Runs())
    {
        AddFlyupsForRun(run);
        AddFlyupsForChains(run);
    }

    // Check for paused.
    if (input::buttons.JustPressed(input::ButtonId::back))
    {
        musicSource_.Pause();
        SetState(State::PAUSED, t);
    }

    // Check for game over.
    if (pit.IsImpacted())
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
        if (input::buttons.JustPressed(input::ButtonId::a) && !pit.IsImpacted())
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
    flyups.erase(std::remove_if(flyups.begin(), flyups.end(), [](const auto& f) { return !f.IsAlive(); }), flyups.end());

    return Screens::Playing;
}


void Playing::DrawBackdrop()
{
    if (!textures.backdrops.empty())
    {
        int index = lastPlayed_ - 1;
        batch_.AddVertices(je::quads::Create(textures.backdrops[index % textures.backdrops.size()], 0.0f, 0.0f));
    }
}


void Playing::DrawPaused()
{
    // Draw a translucent texture over the pit area again.
    batch_.AddVertices(je::quads::Create(textures.blankSquare, topLeft.x, topLeft.y, tileSize * pit.cols, tileSize * (pit.rows - 1)));

    // The game is currently paused.
    {
        const float x = VIRTUAL_WIDTH / 2.0f - 3 * 8.0f;
        const float y = VIRTUAL_HEIGHT / 3.0f;
        textRenderer.DrawLeft(x, y, "Paused", Colours::white, Colours::black);
    }

    if (input::HasGamepad())
    {
        const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 8.0f;
        const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
        textRenderer.DrawLeft(x, y, "(|) quit", Colours::white, Colours::black);
    }
    else
    {
        const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 8.0f;
        const float x = VIRTUAL_WIDTH / 2.0f - 6.125f * 8.0f;
        textRenderer.DrawLeft(x, y, "[ESC]   quit", Colours::white, Colours::black);
    }

    if (input::HasGamepad())
    {
        const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 6.0f;
        const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
        textRenderer.DrawLeft(x, y, "({) play", Colours::white, Colours::black);
    }
    else
    {
        const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 6.0f;
        const float x = VIRTUAL_WIDTH / 2.0f - 6.125f * 8.0f;
        textRenderer.DrawLeft(x, y, "[SPACE] play", Colours::white, Colours::black);
    }
}


void Playing::DrawGameOver(double t)
{
    // Draw a translucent texture over the pit area again.
    batch_.AddVertices(je::quads::Create(textures.blankSquare, topLeft.x, topLeft.y, tileSize * pit.cols, tileSize * (pit.rows - 1)));

    // It's game over, so tell the player.
    if (std::fmod(t - stateStartTime_, 1.0) < 0.6)
    {
        const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f;
        if (pit.IsImpacted())
        {
            const float x = VIRTUAL_WIDTH / 2.0f - 5.0 * 8.0f;
            textRenderer.DrawLeft(x, y, "GAME OVER!", Colours::white, Colours::black);
        }
        else
        {
            const float x = VIRTUAL_WIDTH / 2.0f - 4.0 * 8.0f;
            textRenderer.DrawLeft(x, y, "YOU WIN!", Colours::white, Colours::black);
        }
    }
    if (actionsEnabled_)
    {
        {
            const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 4.0f;
            if (input::HasGamepad())
            {
                const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
                textRenderer.DrawLeft(x, y, "(}) retry", Colours::white, Colours::black);
            }
            else
            {
                const float x = VIRTUAL_WIDTH / 2.0f - 6.125f * 8.0f;
                textRenderer.DrawLeft(x, y, "[CTRL] retry", Colours::white, Colours::black);
            }
        }
        if (input::HasGamepad())
        {
            const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 8.0f;
            const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
            textRenderer.DrawLeft(x, y, "(|) back", Colours::white, Colours::black);
        }
        else
        {
            const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 8.0f;
            const float x = VIRTUAL_WIDTH / 2.0f - 6.125f * 8.0f;
            textRenderer.DrawLeft(x, y, "[ESC]   back", Colours::white, Colours::black);
        }
        if (!pit.IsImpacted())
        {
            if (input::HasGamepad())
            {
                const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 6.0f;
                const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
                textRenderer.DrawLeft(x, y, "({) next", Colours::white, Colours::black);
            }
            else
            {
                const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 6.0f;
                const float x = VIRTUAL_WIDTH / 2.0f - 6.125f * 8.0f;
                textRenderer.DrawLeft(x, y, "[SPACE] next", Colours::white, Colours::black);
            }
        }
    }
}


void Playing::DrawTitle()
{
    const float x = VIRTUAL_WIDTH / 2;
    const float y = 4.0f;
    batch_.AddVertices(je::quads::Create(textures.blankSquare, 0.0f, 2.0f, VIRTUAL_WIDTH, 12.0f));
    textRenderer.DrawCentred(x, y, "Just a minute", Colours::mode, Colours::black);
}


void Playing::DrawStats()
{
    // Draw some stats.
    batch_.AddVertices(je::quads::Create(textures.blankSquare, topLeft.x - tileSize * 3 - tileSize * 0.5f, topLeft.y + tileSize * 2 - tileSize * 0.5f, tileSize * 3, tileSize * 2));
    batch_.AddVertices(je::quads::Create(textures.blankSquare, topLeft.x + tileSize * (pit.cols + 1) - tileSize * 0.5f, topLeft.y + tileSize * 2 - tileSize * 0.5f, tileSize * 5, tileSize * 6));
    timeRenderer.Draw({ topLeft.x - tileSize * 3, topLeft.y + tileSize * 2 }, remainingTime_);
    scoreRenderer.Draw({ topLeft.x + tileSize * (pit.cols + 2.5f), topLeft.y + tileSize * 2 }, score);
    highScoreRenderer.Draw({ topLeft.x + tileSize * (pit.cols + 2.5f), topLeft.y + tileSize * 4 }, highScore_);
    speedRenderer.Draw({ topLeft.x + tileSize * (pit.cols + 2.5f), topLeft.y + tileSize * 6 }, lastPlayed_);
}


void Playing::DrawGui()
{
    DrawTitle();
    DrawStats();
}


void Playing::DrawPit()
{
    // Draw a translucent texture over the pit area, then draw the pit itself.
    batch_.AddVertices(je::quads::Create(textures.blankSquare, topLeft.x, topLeft.y, tileSize * pit.cols, tileSize * (pit.rows - 1)));
    pitRenderer.Draw(topLeft, internalTileScroll, bottomRow);
}


void Playing::DrawCursor()
{
    // We're still playing, so draw the cursor.
    float cursorX = topLeft.x + cursorTileX * tileSize - 1.0f;
    float cursorY = topLeft.y + cursorTileY * tileSize - 1.0f - internalTileScroll;
    batch_.AddVertices(je::quads::Create(textures.cursorTile, cursorX, cursorY));
    batch_.AddVertices(je::quads::Create(textures.cursorTile, cursorX + tileSize, cursorY));
}


void Playing::DrawFlyups()
{
    // Draw fly-ups.
    for (auto& flyup : flyups)
    {
        if (flyup.IsAlive())
        {
            flyup.Draw(batch_);
        }
    }
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

    DrawFlyups();
}
