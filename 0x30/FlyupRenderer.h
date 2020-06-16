#pragma once

#include "Flyup.h"
#include "Pit.h"
#include "Textures.h"

#include "je/Batch.h"

#include <vector>

class FlyupRenderer
{
public:
    FlyupRenderer(const Textures& textures, je::Batch& batch)
        : textures_{textures}, batch_{batch}
    {
    }

    void AddFlyupsForRun(const Pit::RunInfo& run, je::Vec2f topLeft, float tileSize, float internalTileScroll);
    void AddFlyupsForChains(const Pit::RunInfo& run, je::Vec2f topLeft, float tileSize, float internalTileScroll);
    void DrawFlyups();
    void Update();
    void Reset();

private:
    const Textures& textures_;
    je::Batch& batch_;
    std::vector<Flyup> flyups_;
};
