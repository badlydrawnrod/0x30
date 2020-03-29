#include "Playing.h"

#include "Input.h"

#include "je/Logger.h"


Playing::Playing(je::Batch& batch, Textures& textures, std::function<int(int, int)>& rnd) :
    batch_{ batch },
    textures{ textures },
    pit{ rnd },
    pitRenderer{ pit, textures, batch },
    textRenderer{ textures.textTiles, batch },
    timeRenderer{ textRenderer },
    scoreRenderer{ textRenderer },
    speedRenderer{ textRenderer },
    state_{ State::PLAYING },
    elapsed_{ 0 }
{
}


void Playing::Start()
{
    pit.Reset();
    state_ = State::PLAYING;
    score = 0;
    elapsed_ = 0;
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
        LOG("Score: " << score);
    }
}


Screens Playing::Update()
{
    // Update elapsed time only when playing.
    double multiplier = (state_ == State::PLAYING) ? 1.0 : 0.0;
    double now = je::GetTime();
    double delta = (now - lastTime_) * multiplier;
    lastTime_ = now;
    elapsed_ += delta;

    if (state_ == State::PLAYING)
    {
        // Scroll the contents of the pit up.
        internalTileScroll += input::fillHeld ? 1.0f : scrollRate;
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
        if (input::leftPressed && !input::wasLeftPressed && cursorTileX > 0)
        {
            --cursorTileX;
        }
        if (input::rightPressed && !input::wasRightPressed && cursorTileX < Pit::cols - 2)
        {
            ++cursorTileX;
        }
        if (input::upPressed && !input::wasUpPressed && cursorTileY > 1)
        {
            --cursorTileY;
        }
        if (input::downPressed && !input::wasDownPressed && cursorTileY < Pit::rows - 2)
        {
            ++cursorTileY;
        }

        // Swap tiles.
        if (input::swapPressed && !input::wasSwapPressed)
        {
            pit.Swap(cursorTileX, cursorTileY);
        }

        pit.Update();

        UpdateScore(pit, score);

        // Add fly-ups.
        for (const auto& run : pit.Runs())
        {
            AddFlyupsForRun(run);
            AddFlyupsForChains(run);
        }

        // Check for paused.
        if (!input::menuPressed && input::wasMenuPressed)
        {
            state_ = State::PAUSED;
        }

        // Check for game over.
        if (pit.IsImpacted())
        {
            state_ = State::GAME_OVER;
        }

    }
    else if (state_ == State::GAME_OVER)
    {
        if (input::wasMenuPressed && !input::menuPressed)
        {
            return Screens::Menu;
        }
    }
    else if (state_ == State::PAUSED)
    {
        if (input::wasMenuPressed && !input::menuPressed)
        {
            return Screens::Menu;
        }
        if (input::wasSwapPressed && !input::swapPressed)
        {
            state_ = State::PLAYING;
        }
    }

    // Remove dead fly-ups.
    flyups.erase(std::remove_if(flyups.begin(), flyups.end(), [](const auto& f) { return !f.IsAlive(); }), flyups.end());

    return Screens::Playing;
}


void Playing::Draw()
{
    // Draw the backdrop.
    batch_.AddVertices(je::quads::Create(textures.backdrops[3], 0.0f, 0.0f));

    // Draw a translucent texture over the pit area, then draw the pit itself.
    batch_.AddVertices(je::quads::Create(textures.blankTile, topLeft.x, topLeft.y, tileSize * pit.cols, tileSize * pit.rows));
    pitRenderer.Draw(topLeft, internalTileScroll, lastRow, bottomRow);

    // Draw some stats.
    batch_.AddVertices(je::quads::Create(textures.blankTile, topLeft.x - tileSize * 3 - tileSize * 0.5f, topLeft.y + tileSize * 2 - tileSize * 0.5f, tileSize * 3, tileSize * 2));
    batch_.AddVertices(je::quads::Create(textures.blankTile, topLeft.x + tileSize * (pit.cols + 1) - tileSize * 0.5f, topLeft.y + tileSize * 2 - tileSize * 0.5f, tileSize * 5, tileSize * 4));
    timeRenderer.Draw({ topLeft.x - tileSize * 3, topLeft.y + tileSize * 2 }, elapsed_);
    scoreRenderer.Draw({ topLeft.x + tileSize * (pit.cols + 2.5f), topLeft.y + tileSize * 2 }, score);
    speedRenderer.Draw({ topLeft.x + tileSize * (pit.cols + 2.5f), topLeft.y + tileSize * 4 });

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
        if (counter % 60 < 40)
        {
            const float x = VIRTUAL_WIDTH / 2.0f - 40.0f;
            const float y = VIRTUAL_HEIGHT / 2.0f - 4.0f - 64.0f;
            textRenderer.Draw(x + 1.0f, y + 1.0f, "GAME OVER!", { 0x00, 0x00, 0x00, 0xff });
            textRenderer.Draw(x, y, "GAME OVER!");
        }
        counter++;
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
