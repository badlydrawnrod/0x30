#include "FlyupRenderer.h"

#include <algorithm>
#include <vector>

void FlyupRenderer::AddFlyupsForRun(const Pit::RunInfo& run, je::Vec2f topLeft, float tileSize, float internalTileScroll)
{
    // Add fly-ups for runs of 4-9.
    if (run.runSize >= 4 && run.runSize < 10)
    {
        float runFlyupDuration = 1.0f;
        je::TextureRegion texture{};

        switch (run.runSize)
        {
        case 4:
            texture = textures_.combo4;
            break;
        case 5:
            texture = textures_.combo5;
            break;
        case 6:
            texture = textures_.combo6;
            break;
        case 7:
            texture = textures_.combo7;
            break;
        case 8:
            texture = textures_.combo8;
            break;
        case 9:
            texture = textures_.combo9;
            break;
        }

        if (run.runSize >= 4 && run.runSize <= 9)
        {
            for (auto i = 0; i < run.runSize; i++)
            {
                float x = run.coord[i].x * tileSize + topLeft.x + tileSize * 0.5f - texture.w * 0.5f;
                float y = run.coord[i].y * tileSize + topLeft.y + tileSize * 0.5f - texture.h * 0.5f - internalTileScroll;
                flyups_.emplace_back(texture, x, y, runFlyupDuration);
            }
        }
    }
}

void FlyupRenderer::AddFlyupsForChains(const Pit::RunInfo& run, je::Vec2f topLeft, float tileSize, float internalTileScroll)
{
    // Add fly-ups for chains of 2-6.
    if (const auto chains = run.chainLength + 1; chains >= 2 && chains < 7)
    {
        const float chainFlyupDuration = 1.5f;
        je::TextureRegion texture{};
        switch (chains)
        {
        case 2:
            texture = textures_.chain2;
            break;
        case 3:
            texture = textures_.chain3;
            break;
        case 4:
            texture = textures_.chain4;
            break;
        case 5:
            texture = textures_.chain5;
            break;
        case 6:
            texture = textures_.chain6;
            break;
        }
        if (chains >= 2 && chains <= 6)
        {
            for (auto i = 0; i < run.runSize; i++)
            {
                float x = run.coord[i].x * tileSize + topLeft.x + tileSize * 0.5f - texture.w * 0.5f;
                float y = run.coord[i].y * tileSize + topLeft.y + tileSize * 0.5f - texture.h * 0.5f - internalTileScroll - tileSize;
                flyups_.emplace_back(texture, x, y, chainFlyupDuration);
            }
        }
    }
}

void FlyupRenderer::DrawFlyups()
{
    // Draw fly-ups.
    for (auto& flyup : flyups_)
    {
        if (flyup.IsAlive())
        {
            flyup.Draw(batch_);
        }
    }
}

void FlyupRenderer::Update()
{
    flyups_.erase(std::remove_if(flyups_.begin(), flyups_.end(), [](const auto& f) { return !f.IsAlive(); }), flyups_.end());
}

void FlyupRenderer::Reset()
{
    flyups_.clear();
}
