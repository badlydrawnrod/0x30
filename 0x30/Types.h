#pragma once

#include <array>

enum class Screens
{
    Quit,
    Dedication,
    Menu,
    Playing
};

struct ScoreRecord
{
    uint64_t score{0};
};

using Scores = std::array<ScoreRecord, 20>;

struct TimeRecord
{
    double time{0};
};

using Times = std::array<TimeRecord, 3>;

enum class Mode
{
    TIMED,
    ENDLESS
};
