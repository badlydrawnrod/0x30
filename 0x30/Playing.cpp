#include "Playing.h"

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
    state_{ State::PLAYING },
    remaining_{ 0 },
    lastTime_{ 0 }
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
    remaining_ = 61.0;  // Let's be generous and give them a fraction more than 60 seconds.
    lastTime_ = t;
    lastPlayed_ = actualLevel;
    scrollRate = 0.025f + (0.005f * (actualLevel - 1));
    cursorTileX = (Pit::cols / 2) - 1;
    cursorTileY = Pit::rows / 2;
    flyups.clear();
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


void Playing::UpdateScore(const Pit& pit, uint64_t& score)
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


Screens Playing::Update(double t, double dt)
{
    // Update elapsed time only when playing.
    double multiplier = (state_ == State::PLAYING) ? 1.0 : 0.0;
    double now = t;
    double delta = (now - lastTime_) * multiplier;
    lastTime_ = now;
    remaining_ -= delta;
    if (remaining_ < 0.0)
    {
        if (state_ == State::PLAYING)
        {
            SetState(State::GAME_OVER, t);
            SetLevel(level_ + 1);
            actionsEnabled_ = false;
        }
        remaining_ = 0.0;
    }

    if (state_ == State::PLAYING)
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
            je::Play(sounds_.blocksSwapping, blocksSwappingSource_);
        }

        pit.Update();

        if (pit.Landed())
        {
            je::Play(sounds_.blocksLanding, blocksLandingSource_);
        }
        if (auto runs = pit.Runs(); runs.size() > 0)
        {
            je::Play(sounds_.blocksPopping, blocksPoppingSource_);
        }

        UpdateScore(pit, score);

        // Add fly-ups.
        for (const auto& run : pit.Runs())
        {
            AddFlyupsForRun(run);
            AddFlyupsForChains(run);
        }

        // Check for paused.
        if (input::buttons.JustPressed(input::ButtonId::back))
        {
            SetState(State::PAUSED, t);
        }

        // Check for game over.
        if (pit.IsImpacted())
        {
            SetState(State::GAME_OVER, t);
            actionsEnabled_ = false;
        }

    }
    else if (state_ == State::GAME_OVER)
    {
        // Don't allow the player to do anything for a short time to prevent them from taking accidental actions.
        const double delay = 2.0;
        actionsEnabled_ = t - stateStartTime_ >= delay;
        if (actionsEnabled_)
        {
            if (input::buttons.JustPressed(input::ButtonId::b))
            {
                progress_.SaveScores(); // TODO: dedup.
                return Screens::Menu;
            }
            if (input::buttons.JustPressed(input::ButtonId::x))
            {
                progress_.SaveScores(); // TODO: dedup.
                Start(t, lastPlayed_);
            }
            if (input::buttons.JustPressed(input::ButtonId::a) && !pit.IsImpacted())
            {
                // Go on to the next level.
                progress_.SaveScores(); // TODO: dedup.
                Start(t, level_);
            }
        }
    }
    else if (state_ == State::PAUSED)
    {
        if (input::buttons.JustPressed(input::ButtonId::b))
        {
            return Screens::Menu;
        }
        if (input::buttons.JustPressed(input::ButtonId::a))
        {
            SetState(State::PLAYING, t);
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
        batch_.AddVertices(je::quads::Create(textures.backdrops[(lastPlayed_ - 1) % textures.backdrops.size()], 0.0f, 0.0f));
    }
}

void Playing::Draw(double t)
{
    // Draw the backdrop.
    DrawBackdrop();

    {
        const float x = VIRTUAL_WIDTH / 2 - 6.5f * 8.0f;
        const float y = 4.0f;
        batch_.AddVertices(je::quads::Create(textures.blankSquare, 0.0f, 2.0f, VIRTUAL_WIDTH, 12.0f));
        textRenderer.Draw(x + 1.0f, y + 1.0f, "Just a minute", { 0x00, 0x00, 0x00, 0xff });
        textRenderer.Draw(x, y, "Just a minute", { 0xff, 0x00, 0x00, 0xff });
    }

    // Draw a translucent texture over the pit area, then draw the pit itself.
    batch_.AddVertices(je::quads::Create(textures.blankSquare, topLeft.x, topLeft.y, tileSize * pit.cols, tileSize * pit.rows));
    pitRenderer.Draw(topLeft, internalTileScroll, lastRow, bottomRow);

    // Draw some stats.
    batch_.AddVertices(je::quads::Create(textures.blankSquare, topLeft.x - tileSize * 3 - tileSize * 0.5f, topLeft.y + tileSize * 2 - tileSize * 0.5f, tileSize * 3, tileSize * 2));
    batch_.AddVertices(je::quads::Create(textures.blankSquare, topLeft.x + tileSize * (pit.cols + 1) - tileSize * 0.5f, topLeft.y + tileSize * 2 - tileSize * 0.5f, tileSize * 5, tileSize * 6));
    timeRenderer.Draw({ topLeft.x - tileSize * 3, topLeft.y + tileSize * 2 }, remaining_);
    scoreRenderer.Draw({ topLeft.x + tileSize * (pit.cols + 2.5f), topLeft.y + tileSize * 2 }, score);
    highScoreRenderer.Draw({ topLeft.x + tileSize * (pit.cols + 2.5f), topLeft.y + tileSize * 4 }, highScore_);
    speedRenderer.Draw({ topLeft.x + tileSize * (pit.cols + 2.5f), topLeft.y + tileSize * 6 }, lastPlayed_);

    if (state_ == State::PLAYING)
    {
        // We're still playing, so draw the cursor.
        float cursorX = topLeft.x + cursorTileX * tileSize - 1.0f;
        float cursorY = topLeft.y + cursorTileY * tileSize - 1.0f - internalTileScroll;
        batch_.AddVertices(je::quads::Create(textures.cursorTile, cursorX, cursorY));
        batch_.AddVertices(je::quads::Create(textures.cursorTile, cursorX + tileSize, cursorY));
    }
    else if (state_ == State::GAME_OVER)
    {
        // It's game over, so tell the player.
        if (std::fmod(t - stateStartTime_, 1.0) < 0.6)
        {
            const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f;
            if (pit.IsImpacted())
            {
                const float x = VIRTUAL_WIDTH / 2.0f - 5.0 * 8.0f;
                textRenderer.Draw(x + 1.0f, y + 1.0f, "GAME OVER!", { 0x00, 0x00, 0x00, 0xff });
                textRenderer.Draw(x, y, "GAME OVER!");
            }
            else
            {
                const float x = VIRTUAL_WIDTH / 2.0f - 4.0 * 8.0f;
                textRenderer.Draw(x + 1.0f, y + 1.0f, "TIME UP!", { 0x00, 0x00, 0x00, 0xff });
                textRenderer.Draw(x, y, "TIME UP!");
            }
        }
        if (actionsEnabled_)
        {
            {
                const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 4.0f;
                if (pit.IsImpacted())
                {
                    const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
                    textRenderer.Draw(x + 1.0f, y + 1.0f, "[X] retry", { 0x00, 0x00, 0x00, 0xff });
                    textRenderer.Draw(x, y, "[X] retry");
                }
                else
                {
                    const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
                    textRenderer.Draw(x + 1.0f, y + 1.0f, "[X] replay", { 0x00, 0x00, 0x00, 0xff });
                    textRenderer.Draw(x, y, "[X] replay");
                }
            }
            {
                const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 8.0f;
                const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
                textRenderer.Draw(x + 1.0f, y + 1.0f, "[B] back", { 0x00, 0x00, 0x00, 0xff });
                textRenderer.Draw(x, y, "[B] back");
            }
            if (!pit.IsImpacted())
            {
                const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f + 8.0f * 6.0f;
                const float x = VIRTUAL_WIDTH / 2.0f - 5.0f * 8.0f;
                textRenderer.Draw(x + 1.0f, y + 1.0f, "[A] next level", { 0x00, 0x00, 0x00, 0xff });
                textRenderer.Draw(x, y, "[A] next level");
            }
        }
    }
    else if (state_ == State::PAUSED)
    {
        // The game is currently paused.
        const float x = VIRTUAL_WIDTH / 2.0f - 3 * 8.0f;
        const float y = VIRTUAL_HEIGHT / 3.0f;
        textRenderer.Draw(x + 1.0f, y + 1.0f, "Paused", { 0x00, 0x00, 0x00, 0xff });
        textRenderer.Draw(x, y, "Paused");
    }

    // Draw fly-ups.
    for (auto& flyup : flyups)
    {
        if (flyup.IsAlive())
        {
            flyup.Draw(batch_);
        }
    }
}
