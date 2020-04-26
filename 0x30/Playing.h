#pragma once

#include "Constants.h"
#include "Flyup.h"
#include "LevelRenderer.h"
#include "Pit.h"
#include "PitRenderer.h"
#include "Progress.h"
#include "ScoreRenderer.h"
#include "Sounds.h"
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
    Playing(Progress& progress, je::Batch& batch, Textures& textures, Sounds& sounds, std::function<int(int, int)>& rnd);

    void Start(double t);
    void Start(double t, int level);
    
    Screens Update(double t, double dt);
    void DrawCursor();
    void Draw(double t);

private:
    enum class State { PLAYING, PAUSED, GAME_OVER };

    Screens UpdateGameOver(double t);
    Screens UpdatePaused(double t);
    void UpdatePlaying(double t);

    void DrawPaused();
    void DrawGameOver(double t);
    void DrawStats();
    void DrawTitle();
    void DrawPit();
    void DrawGui();
    void DrawFlyups();

    void AddFlyupsForRun(const Pit::RunInfo& run);
    void AddFlyupsForChains(const Pit::RunInfo& run);
    void UpdateScore(const Pit& pit, uint64_t& score);
    void DrawBackdrop();

    void SetLevel(int level);
    void SetState(State state, double t);

    const float tileSize = 16.0f;

    Progress& progress_;
    je::Batch& batch_;
    Textures& textures;
    Sounds& sounds_;
    je::SoundSource blocksSwappingSource_;
    je::SoundSource blocksLandingSource_;
    je::SoundSource blocksPoppingSource_;
    je::SoundSource musicSource_;
    Pit pit;
    PitRenderer pitRenderer;
    TextRenderer textRenderer;
    TimeRenderer timeRenderer;
    ScoreRenderer scoreRenderer;
    ScoreRenderer highScoreRenderer;
    LevelRenderer speedRenderer;
    
    double lastTime_{ 0.0 };
    double elapsedTime_{ 0.0 };
    double remainingTime_{ 0.0 };
    double stateStartTime_{ 0.0 };

    State state_;
    bool actionsEnabled_{ false };

    const je::Vec2f topLeft{ (VIRTUAL_WIDTH - Pit::cols * tileSize) / 2.0f, VIRTUAL_HEIGHT - Pit::rows * tileSize };
    const float bottomRow{ topLeft.y + (Pit::rows - 1) * tileSize };
    const float lastRow{ bottomRow - tileSize };

    float internalTileScroll{ 0.0f };
    float scrollRate{ 0.025f };

    int cursorTileX{ (Pit::cols / 2) - 1 };
    int cursorTileY{ Pit::rows / 2 };

    uint64_t score{ 0 };
    uint64_t highScore_{ 0 };
    int level_{ 1 };
    int lastPlayed_{ 1 };
    static constexpr size_t numLevels = std::tuple_size<Scores>::value;

    std::vector<Flyup> flyups;
};
