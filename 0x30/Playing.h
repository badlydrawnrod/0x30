#pragma once

#include "Buttons.h"
#include "Constants.h"
#include "FlyupRenderer.h"
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
#include "je/MyTime.h"
#include "je/QuadHelpers.h"

#include <array>
#include <functional>
#include <iomanip>
#include <sstream>

class Playing
{
public:
    Playing(Buttons& buttonState, Progress& progress, je::Batch& batch, Textures& textures, Sounds& sounds, std::function<int(int, int)>& rnd);

    void SetDifficulty(size_t actualLevel);

    void Start(double t, size_t level, Mode mode);

    Screens Update(double t, double dt);
    void DrawCursor();
    void Draw(double t);

private:
    enum class State
    {
        PLAYING,
        PAUSED,
        GAME_OVER
    };

    Screens UpdateGameOver(double t);
    Screens UpdatePaused(double t);
    void UpdatePlaying(double t);

    void DrawPaused();
    void DrawGameOver(double t);
    void DrawStats();
    void DrawTitle();
    void DrawPit();
    void DrawGui();

    void UpdateScore();
    void DrawBackdrop();

    void SetLevel(size_t level);
    void SetState(State state, double t);

    const float tileSize_ = 16.0f;

    Buttons& buttons_;
    Progress& progress_;
    je::Batch& batch_;
    Textures& textures_;
    Sounds& sounds_;
    je::SoundSource blocksSwappingSource_;
    je::SoundSource blocksLandingSource_;
    je::SoundSource blocksPoppingSource_;
    je::SoundSource musicSource_;
    Pit pit_;
    PitRenderer pitRenderer_;
    TextRenderer textRenderer_;
    TimeRenderer timeRenderer_;
    TimeRenderer bestTimeRenderer_;
    ScoreRenderer scoreRenderer_;
    ScoreRenderer highScoreRenderer_;
    LevelRenderer speedRenderer_;
    FlyupRenderer flyupRenderer_;
    Mode mode_;

    double lastTime_{0.0};
    double elapsedTime_{0.0};
    double stateStartTime_{0.0};
    double remainingTime_{0.0};
    double timeToNextLevelChange_{0.0};

    State state_;
    bool actionsEnabled_{false};

    const je::Vec2f topLeft_{(VIRTUAL_WIDTH - Pit::cols * tileSize_) / 2.0f, VIRTUAL_HEIGHT - Pit::rows* tileSize_};
    const float bottomRow_{topLeft_.y + (Pit::rows - 1) * tileSize_};

    float internalTileScroll_{0.0f};
    float scrollRate_{0.025f};

    size_t cursorTileX_{(Pit::cols / 2) - 1};
    size_t cursorTileY_{Pit::rows / 2};

    uint64_t score_{0};
    uint64_t highScore_{0};
    double bestTime_{0.0};
    size_t level_{1};
    size_t lastPlayed_{1};
    size_t initialLevel_{1};
    static constexpr size_t numLevels_ = std::tuple_size<Scores>::value;
};
