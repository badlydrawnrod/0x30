#pragma once

#include "Assets.h"
#include "Constants.h"
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


class Flyup
{
public:
    Flyup(je::TextureRegion& texture, float x, float y, float duration);
    Flyup(const Flyup& other);
    Flyup& operator=(const Flyup& other);

    void Draw(je::Batch& batch);
    bool IsAlive() const;

private:
    je::TextureRegion texture_;
    float x_;
    float y_;
    double endTime_;
};


inline Flyup::Flyup(je::TextureRegion& texture, float x, float y, float duration) : texture_{ texture }, x_{ x }, y_{ y }, endTime_{ je::GetTime() + duration }
{
}

inline Flyup::Flyup(const Flyup& other) : texture_{ other.texture_ }, x_{ other.x_ }, y_{ other.y_ }, endTime_{ other.endTime_ }
{
}

inline Flyup& Flyup::operator=(const Flyup& other)
{
    texture_ = other.texture_;
    x_ = other.x_;
    y_ = other.y_;
    endTime_ = other.endTime_;

    return *this;
}

inline void Flyup::Draw(je::Batch& batch)
{
    batch.AddVertices(je::quads::Create(texture_, x_, y_));
    y_ -= 0.25f;
}


inline bool Flyup::IsAlive() const
{
    return je::GetTime() < endTime_;
}



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
