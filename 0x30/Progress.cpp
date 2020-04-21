#include "Progress.h"

#include <json/json.h>

#include <filesystem>


namespace
{
    constexpr const char* scoresFile = "data/progress.json";

    Scores defaultScores{
        ScoreRecord{1000, "Rod"},
        ScoreRecord{1000, "Rod"},
        ScoreRecord{1000, "Rod"},
        ScoreRecord{1000, "Rod"},
        ScoreRecord{1000, "Rod"},
        ScoreRecord{1000, "Rod"},
        ScoreRecord{1000, "Rod"},
        ScoreRecord{1000, "Rod"},
        ScoreRecord{1000, "Rod"},
        ScoreRecord{1000, "Rod"}
    };

    void ToJson(Json::Value& jsonProgress, const Scores& scores, int maxLevel)
    {
        Json::Value jsonScores;
        for (auto [score, name] : scores)
        {
            Json::Value jsonScore;
            jsonScore["score"] = score;
            jsonScore["name"] = name;
            jsonScores.append(jsonScore);
        }
        jsonProgress["scores"] = jsonScores;
        jsonProgress["maxLevel"] = maxLevel;
    }

    void FromJson(const Json::Value& jsonProgress, Scores& scores, int& maxLevel)
    {
        const Json::Value& jsonScores = jsonProgress["scores"];
        size_t level = 0;
        for (const Json::Value& jsonScore : jsonScores)
        {
            scores[level].score = jsonScore["score"].asUInt64();
            scores[level].name = jsonScore["name"].asString();
            ++level;
        }
        maxLevel = jsonProgress["maxLevel"].asInt();
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
        return;
    }

    std::ifstream infile(scoresFile);
    Json::Value root;
    infile >> root;
    FromJson(root, scores_, maxLevel_);
}


void Progress::SaveScores()
{
    std::filesystem::create_directory("data");
    std::ofstream outfile(scoresFile);
    Json::Value root;
    ToJson(root, scores_, maxLevel_);
    outfile << root << std::endl;
}
