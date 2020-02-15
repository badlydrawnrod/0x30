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
    void FindVerticalRuns(bool& foundRun);
    void FindHorizontalRuns(bool& foundRun);
    void FindAdjacentVerticalRuns(bool& foundRun);
    void FindAdjacentHorizontalRuns(bool& foundRun);
    void CheckForRuns();
    void RemoveRuns();
    void RefillBottomRow();
    void ScrollOne();

    void Swap(size_t x, size_t y);
    Tile TileAt(size_t x, size_t y) const;
    size_t PitIndex(size_t x, size_t y) const;

private:
    std::array<Tile, cols * rows> tiles_;
    std::array<bool, cols * rows> runs_;
    size_t firstRow_ = 0;
    std::function<int(int, int)>& rnd_;
};
