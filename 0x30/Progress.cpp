#include "Progress.h"

#include "je/AsyncPersistence.h"
#include "je/Logger.h"

#include <sstream>

const int progressFileVersion = 2;

namespace
{
    constexpr const char* scoresFile = "data/progress.json";

    Scores defaultScores{
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500},
            ScoreRecord{500}};

    Times defaultTimes{
            TimeRecord{300},
            TimeRecord{300},
            TimeRecord{300}};
} // namespace

Progress::Progress()
{
    LoadScores();
}

void Progress::LoadScores()
{
    je::AsyncPersistenceLoader::Load(
            "scores", [this](auto filename, void* data, int length) {
                std::stringstream ss{reinterpret_cast<char*>(data)};
                ss >> maxLevel_;
                for (int i = 0; i < scores_.size(); i++)
                {
                    ss >> scores_[i].score;
                    LOG("Loaded score " << i << " = " << scores_[i].score);
                }
                ss >> maxTimedLevel_;
                for (int i = 0; i < times_.size(); i++)
                {
                    ss >> times_[i].time;
                }
                LOG("Loaded scores from " << filename);
            },
            [this](auto filename) {
                LOG("Failed to load scores from " << filename);
                maxLevel_ = 1;
                scores_ = defaultScores;
                times_ = defaultTimes;
                maxTimedLevel_ = 1;
                SaveScores();
            });
}

void Progress::SaveScores()
{
    std::stringstream ss;
    ss << maxLevel_ << '\n';
    for (auto score : scores_)
    {
        ss << score.score << '\n';
    }
    ss << maxTimedLevel_ << '\n';
    for (auto time : times_)
    {
        ss << time.time << '\n';
    }
    placeholderScores_ = ss.str();

    je::AsyncPersistenceSaver::Save(
            "scores",
            placeholderScores_.data(),
            placeholderScores_.size(),
            [](auto filename) {
                LOG("Saved scores to " << filename);
            },
            [](auto filename) {
                LOG("Failed to save scores to " << filename);
            });
}
