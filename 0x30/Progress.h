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

    void UpdateHighScore(int level, uint64_t score);
    uint64_t HighScore(int level) const;

private:
    int maxLevel_{ 1 };
    Scores scores_;

    int maxTimedLevel_{ 1 };
    Times times_;
};


inline void Progress::UpdateHighScore(int level, uint64_t score)
{
    int index = level - 1;
    scores_[index].score = std::max(score, scores_[index].score);
}


inline uint64_t Progress::HighScore(int level) const
{
    int index = level - 1;
    return scores_[index].score;
}
