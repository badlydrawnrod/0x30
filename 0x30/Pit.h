#pragma once

#include <array>
#include <functional>


class Pit
{
public:
    static constexpr size_t cols = 6;
    static constexpr size_t rows = 13;  // Note, one more than is visible because of the wraparound.

    enum class Tile { None, Red, Green, Yellow, Cyan, Magenta, Wall };

public:
    Pit(std::function<int(int, int)>& rnd);

    void ApplyGravity();
    void CheckForVerticalRun(const size_t x, const size_t y, bool& foundRun);
    void CheckForVerticalRuns(bool& foundRun);
    void CheckForHorizontalRun(const size_t x, const size_t y, bool& foundRun);
    void CheckForHorizontalRuns(bool& foundRun);
    void CheckForAdjacentRunVertically(const size_t x, const size_t y, bool& foundRun);
    void CheckForAdjacentRunsVertically(bool& foundRun);
    void CheckForAdjacentRunHorizontally(const size_t x, const size_t y, bool& foundRun);
    void CheckForAdjacentRunsHorizontally(bool& foundRun);
    void CheckForRuns();
    void RemoveRuns();
    void RefillBottomRow();
    void ScrollOne();

    void Swap(size_t x, size_t y);
    bool& RunAt(size_t x, size_t y);
    int HeightAt(size_t x, size_t y) const;
    Tile TileAt(size_t x, size_t y) const;
    size_t PitIndex(size_t x, size_t y) const;
    bool IsImpacted() const { return impacted_; }

private:
    std::array<Tile, cols * rows> tiles_;
    std::array<bool, cols * rows> runs_;
    std::array<int, cols * rows> heights_;
    size_t firstRow_ = 0;
    std::function<int(int, int)>& rnd_;
    bool impacted_;
};
