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

    void UpdateMaxLevel(size_t level)
    {
        maxLevel_ = std::max(level, maxLevel_);
    }

    size_t MaxLevel() const
    {
        return maxLevel_;
    }

    size_t MaxTimedLevel() const
    {
        return maxTimedLevel_;
    }

    Scores LevelScores() const
    {
        return scores_;
    }

    Times LevelTimes() const
    {
        return times_;
    }

    void UpdateHighScore(size_t level, uint64_t score);
    uint64_t HighScore(size_t level) const;

    void UpdateBestTime(size_t level, double time);
    double BestTime(size_t level) const;

private:
    size_t maxLevel_{1};
    Scores scores_;

    size_t maxTimedLevel_{1};
    Times times_;
};

inline void Progress::UpdateHighScore(size_t level, uint64_t score)
{
    size_t index = level - 1;
    scores_[index].score = std::max(score, scores_[index].score);
}

inline uint64_t Progress::HighScore(size_t level) const
{
    size_t index = level - 1;
    return scores_[index].score;
}

inline void Progress::UpdateBestTime(size_t level, double time)
{
    size_t index = level - 1;
    times_[index].time = std::max(time, times_[index].time);
}

inline double Progress::BestTime(size_t level) const
{
    size_t index = level - 1;
    return times_[index].time;
}
