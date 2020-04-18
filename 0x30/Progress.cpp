#include "Progress.h"


namespace
{
    Scores defaultScores{
        ScoreRecord{100, "Rod"},
        ScoreRecord{100, "Rod"},
        ScoreRecord{100, "Rod"},
        ScoreRecord{100, "Rod"},
        ScoreRecord{100, "Rod"},
        ScoreRecord{100, "Rod"},
        ScoreRecord{100, "Rod"},
        ScoreRecord{100, "Rod"},
        ScoreRecord{100, "Rod"},
        ScoreRecord{100, "Rod"}
    };

    Scores LoadScores()
    {
        return defaultScores;
    }
}


Progress::Progress() : scores_{ LoadScores() }
{
}
