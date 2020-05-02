#pragma once

#include <array>

enum class Screens { Quit, Dedication, Menu, Playing };

struct ScoreRecord
{
    uint64_t score{ 0 };
    std::string name;
};

using Scores = std::array<ScoreRecord, 20>;

enum class Mode { TIMED, ENDLESS };
