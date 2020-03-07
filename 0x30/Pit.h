#pragma once

#include <array>
#include <functional>


class Pit
{
public:
    static constexpr size_t cols = 6;
    static constexpr size_t rows = 13;  // Note, one more than is visible because of the wraparound.

    enum class Tile { None, Red, Green, Yellow, Cyan, Magenta, Wall };

    struct RunInfo
    {
        size_t runSize;
        size_t chainLength;
    };

public:
    Pit(std::function<int(int, int)>& rnd);

    void Update();
    void ScrollOne();
    void Swap(size_t x, size_t y);

    size_t& RunAt(size_t x, size_t y);
    int HeightAt(size_t x, size_t y) const;
    Tile TileAt(size_t x, size_t y) const;
    size_t PitIndex(size_t x, size_t y) const;

    bool IsImpacted() const { return impacted_; }
    const std::vector<RunInfo>& Runs() const { return runSizes_; }

private:
    void ApplyGravity();
    void CheckForRuns();
    void RemoveRuns();
    void RemoveDeadChains();
    void RefillBottomRow();

    void CheckForAdjacentRunVertically(const size_t x, const size_t y);
    void CheckForAdjacentRunsVertically();
    void CheckForAdjacentRunHorizontally(const size_t x, const size_t y);
    void CheckForAdjacentRunsHorizontally();
    void CheckForVerticalRun(const size_t x, const size_t y, bool& foundRun);
    void CheckForVerticalRuns(bool& foundRun);
    void CheckForHorizontalRun(const size_t x, const size_t y, bool& foundRun);
    void CheckForHorizontalRuns(bool& foundRun);

    std::array<Tile, cols * rows> tiles_;
    std::array<size_t, cols * rows> runs_;
    std::array<int, cols * rows> heights_;
    std::array<size_t, cols * rows> chains_;
    size_t firstRow_ = 0;
    std::function<int(int, int)>& rnd_;
    bool impacted_;
    size_t run_;
    std::vector<RunInfo> runSizes_;
};
