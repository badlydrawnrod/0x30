#include "Progress.h"

#include <json/json.h>

#include <filesystem>


const int progressFileVersion = 1;


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
        ScoreRecord{500}
    };

    Times defaultTimes{
        TimeRecord{300},
        TimeRecord{300},
        TimeRecord{300}
    };

    void ToJson(Json::Value& jsonProgress, const Scores& scores, int maxLevel, const Times& times, int maxTimedLevel)
    {
        jsonProgress["version"] = progressFileVersion;

        Json::Value jsonScores;
        for (auto [score] : scores)
        {
            Json::Value jsonScore;
            jsonScore["score"] = score;
            jsonScores.append(jsonScore);
        }
        Json::Value timed;
        timed["scores"] = jsonScores;
        timed["maxLevel"] = maxLevel;
        jsonProgress["timed"] = timed;

        Json::Value jsonTimes;
        for (auto [time] : times)
        {
            Json::Value jsonTime;
            jsonTime["time"] = time;
            jsonTimes.append(jsonTime);
        }
        Json::Value endless;
        endless["times"] = jsonTimes;
        endless["maxLevel"] = maxTimedLevel;
        jsonProgress["endless"] = endless;
    }

    void FromJson(const Json::Value& jsonProgress, Scores& scores, int& maxLevel, Times& times, int& maxTimedLevel)
    {
        if (jsonProgress.isMember("version") && jsonProgress["version"].asInt() == 1)
        {
            // Load version 1 of the save file.
            const Json::Value& timed = jsonProgress["timed"];
            const Json::Value& jsonScores = timed["scores"];
            size_t level = 0;
            for (const Json::Value& jsonScore : jsonScores)
            {
                scores[level].score = jsonScore["score"].asUInt64();
                ++level;
            }
            maxLevel = timed["maxLevel"].asInt();

            const Json::Value& endless = jsonProgress["endless"];
            const Json::Value& jsonTimes = endless["times"];
            level = 0;
            for (const Json::Value& jsonTime : jsonTimes)
            {
                times[level].time = jsonTime["time"].asDouble();
                ++level;
            }
            maxTimedLevel = endless["maxLevel"].asInt();
        }
        else
        {
            // Load the original (unversioned) save file.
            const Json::Value& jsonScores = jsonProgress["scores"];
            size_t level = 0;
            for (const Json::Value& jsonScore : jsonScores)
            {
                scores[level].score = jsonScore["score"].asUInt64();
                ++level;
            }
            maxLevel = jsonProgress["maxLevel"].asInt();

            times = defaultTimes;
            maxTimedLevel = static_cast<int>(times.size() + 1);
        }
    }
}


Progress::Progress()
{
    LoadScores();
}


void Progress::LoadScores()
{
    if (!std::filesystem::exists(scoresFile))
    {
        scores_ = defaultScores;
        maxLevel_ = 1;
        times_ = defaultTimes;
        maxTimedLevel_ = 1;
        return;
    }

    std::ifstream infile(scoresFile);
    Json::Value root;
    infile >> root;
    FromJson(root, scores_, maxLevel_, times_, maxTimedLevel_);
}


void Progress::SaveScores()
{
    std::filesystem::create_directory("data");
    std::ofstream outfile(scoresFile);
    Json::Value root;
    ToJson(root, scores_, maxLevel_, times_, maxTimedLevel_);
    outfile << root << std::endl;
}
