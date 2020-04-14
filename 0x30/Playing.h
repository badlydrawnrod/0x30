#pragma once

#include "Constants.h"
#include "Flyup.h"
#include "Pit.h"
#include "PitRenderer.h"
#include "ScoreRenderer.h"
#include "Sounds.h"
#include "SpeedRenderer.h"
#include "TextRenderer.h"
#include "Textures.h"
#include "TimeRenderer.h"
#include "Types.h"

#include "je/Batch.h"
#include "je/QuadHelpers.h"
#include "je/Time.h"

#include <array>
#include <functional>
#include <iomanip>
#include <sstream>


class Playing
{
public:
    Playing(je::Batch& batch, Textures& textures, Sounds& sounds, std::function<int(int, int)>& rnd);

    void Start(double t);
    void Start(double t, int level);
    Screens Update(double t, double dt);
    void Draw(double t);

    int MaxLevel() const { return maxLevel_; }
    Scores LevelScores() const { return scores_; }

private:

    enum class State { PLAYING, PAUSED, GAME_OVER };

    void AddFlyupsForRun(const Pit::RunInfo& run);
    void AddFlyupsForChains(const Pit::RunInfo& run);
    void UpdateScore(const Pit& pit, uint64_t& score);
    void DrawBackdrop();

    void SetLevel(int level);
    void SetState(State state, double t);

    const float tileSize = 16.0f;

    je::Batch& batch_;
    Textures& textures;
    Sounds& sounds_;
    je::SoundSource blocksSwappingSource_;
    je::SoundSource blocksLandingSource_;
    je::SoundSource blocksPoppingSource_;
    Pit pit;
    PitRenderer pitRenderer;
    TextRenderer textRenderer;
    TimeRenderer timeRenderer;
    ScoreRenderer scoreRenderer;
    ScoreRenderer highScoreRenderer;
    SpeedRenderer speedRenderer;
    double lastTime_;
    double remaining_;
    State state_;
    double stateStartTime_{ 0.0 };
    bool actionsEnabled_{ false };

    const je::Vec2f topLeft{ (VIRTUAL_WIDTH - Pit::cols * tileSize) / 2.0f, VIRTUAL_HEIGHT - Pit::rows * tileSize };
    const float bottomRow{ topLeft.y + (Pit::rows - 1) * tileSize };
    const float lastRow{ bottomRow - tileSize };

    float internalTileScroll{ 0.0f };
    float scrollRate{ 0.025f };

    int cursorTileX{ (Pit::cols / 2) - 1 };
    int cursorTileY{ Pit::rows / 2 };

    uint64_t score{ 0 };
    int level_{ 1 };
    int lastPlayed_{ 1 };
    int maxLevel_{ 1 };
    static constexpr size_t numLevels = 10;
    Scores scores_{};

    std::vector<Flyup> flyups;
};
