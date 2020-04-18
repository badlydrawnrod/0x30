#pragma once

#include <array>

enum class Screens { Quit, Dedication, Menu, Playing };

struct ScoreRecord
{
    uint64_t score;
    std::string name;
};

using Scores = std::array<ScoreRecord, 10>;