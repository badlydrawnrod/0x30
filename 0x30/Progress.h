#pragma once

#include "Types.h"

#include <algorithm>
#include <fstream>



class Progress
{
public:
    Progress();

    void LoadScores();
    void SaveScores();

    void UpdateMaxLevel(int level) { maxLevel_ = std::max(level, maxLevel_); }
    int MaxLevel() const { return maxLevel_; }

    Scores LevelScores() const { return scores_; }

    void UpdateHighScore(int level, uint64_t score) { scores_[level - 1].score = std::max(score, scores_[level - 1].score); }
    uint64_t HighScore(int level) const { return scores_[level - 1].score; }

private:
    int maxLevel_{ 1 };
    Scores scores_;
};
