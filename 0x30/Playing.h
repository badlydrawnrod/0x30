#pragma once

#include "Assets.h"
#include "Constants.h"
#include "Flyup.h"
#include "Pit.h"
#include "PitRenderer.h"
#include "ScoreRenderer.h"
#include "SpeedRenderer.h"
#include "TextRenderer.h"
#include "TimeRenderer.h"

#include "je/Batch.h"
#include "je/QuadHelpers.h"
#include "je/Time.h"

#include <functional>
#include <iomanip>
#include <sstream>


class Playing
{
public:
    Playing(je::Batch& batch, Textures& textures, std::function<int(int, int)>& rnd);

    void Update();
    void Draw();

private:
    void AddFlyupsForRun(const Pit::RunInfo& run);
    void AddFlyupsForChains(const Pit::RunInfo& run);
    void UpdateScore(const Pit& pit, uint64_t& score);

    const float tileSize = 16.0f;

    je::Batch& batch;
    Textures& textures;
    Pit pit;
    PitRenderer pitRenderer;
    TextRenderer textRenderer;
    TimeRenderer timeRenderer;
    ScoreRenderer scoreRenderer;
    SpeedRenderer speedRenderer;
    double startTime{ je::GetTime() };

    const je::Vec2f topLeft{ (VIRTUAL_WIDTH - Pit::cols * tileSize) / 2.0f, VIRTUAL_HEIGHT - Pit::rows * tileSize };
    const float bottomRow{ topLeft.y + (Pit::rows - 1) * tileSize };
    const float lastRow{ bottomRow - tileSize };

    float internalTileScroll{ 0.0f };
    float scrollRate{ 0.025f };

    int cursorTileX{ (Pit::cols / 2) - 1 };
    int cursorTileY{ Pit::rows / 2 };

    size_t counter{ 0 };
    uint64_t score{ 0 };
    std::vector<Flyup> flyups;
};
