#pragma once

#include <array>
#include <functional>


class Pit
{
public:
    static constexpr size_t cols = 6;
    static constexpr size_t rows = 13;  // Note, one more than is visible because of the wraparound.

    enum class TileType { None, Red, Green, Yellow, Cyan, Magenta, Wall };

    struct PitCoord
    {
        size_t x;
        size_t y;
    };

    struct RunInfo
    {
        size_t runSize;
        size_t chainLength;
        std::vector<PitCoord> coord;
    };

public:
    Pit(std::function<int(int, int)>& rnd);

    void Update();
    void ScrollOne();
    void Swap(size_t x, size_t y);

    int HeightAt(size_t x, size_t y) const;
    TileType TileTypeAt(size_t x, size_t y) const;

    bool IsImpacted() const { return impacted_; }
    const std::vector<RunInfo>& Runs() const { return runInfo_; }

private:
    size_t PitIndex(size_t x, size_t y) const;

    void LowerHeight(size_t x, size_t y);
    void MoveDown(size_t x, size_t y);

    void PutTile(size_t x, size_t y, TileType type);

    size_t& RunAt(size_t x, size_t y);
    TileType TileAt(size_t x, size_t y) const;
    size_t& ChainAt(size_t x, size_t y);

    bool IsEmpty(size_t x, size_t y) const;
    bool IsMovable(size_t x, size_t y) const;


    void ApplyGravity();
    void CheckForRuns();
    void RemoveRuns();
    void RemoveDeadChains();
    void RefillBottomRow();

    bool CheckForAdjacentRunVertically(const size_t x, const size_t y);
    bool CheckForAdjacentRunsVertically();
    bool CheckForAdjacentRunHorizontally(const size_t x, const size_t y);
    bool CheckForAdjacentRunsHorizontally();
    void CheckForVerticalRun(const size_t x, const size_t y, bool& foundRun);
    void CheckForVerticalRuns(bool& foundRun);
    void CheckForHorizontalRun(const size_t x, const size_t y, bool& foundRun);
    void CheckForHorizontalRuns(bool& foundRun);

    std::array<TileType, cols * rows> tiles_;
    std::array<size_t, cols * rows> runs_;
    std::array<int, cols * rows> heights_;
    std::array<size_t, cols * rows> chains_;
    size_t firstRow_ = 0;
    std::function<int(int, int)>& rnd_;
    bool impacted_;
    size_t run_;
    std::vector<RunInfo> runInfo_;
};
