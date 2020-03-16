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

    struct Tile
    {
        TileType tileType{ TileType::None };
        size_t runId{ 0 };
        int height{ 0 };
        size_t chain{ 0 };

        Tile() : Tile{ TileType::None } {}

        Tile(TileType tileType) : tileType{ tileType }
        {
        }

        bool IsEmpty() const
        {
            return tileType == TileType::None;
        }

        bool IsMovableType() const
        {
            return tileType != TileType::Wall;
        }

        bool IsFixedType() const
        {
            return tileType == TileType::Wall;
        }

        bool IsInRun() const
        {
            return runId != 0;
        }

        bool IsDescended() const
        {
            return height == 0;
        }
    };

public:
    Pit(std::function<int(int, int)>& rnd);

    void Update();
    void ScrollOne();
    void Swap(size_t x, size_t y);

    int HeightAt(size_t x, size_t y) const
    {
        return tiles_[PitIndex(x, y)].height;
    }

    TileType TileTypeAt(size_t x, size_t y) const
    {
        return tiles_[PitIndex(x, y)].tileType;
    }

    bool IsImpacted() const { return impacted_; }
    const std::vector<RunInfo>& Runs() const { return runInfo_; }

private:
    size_t PitIndex(size_t x, size_t y) const
    {
        size_t col = x % cols;
        size_t row = (y + firstRow_) % rows;
        return col + row * cols;
    }

    Tile& TileAt(size_t x, size_t y)
    {
        return tiles_[PitIndex(x, y)];
    }

    const Tile& TileAt(size_t x, size_t y) const
    {
        return tiles_[PitIndex(x, y)];
    }

    void ClearTile(size_t x, size_t y)
    {
        TileAt(x, y) = Tile{};
    }

    size_t& RunAt(size_t x, size_t y)
    {
        return TileAt(x, y).runId;
    }

    size_t& ChainAt(size_t x, size_t y)
    {
        return tiles_[PitIndex(x, y)].chain;
    }

    bool IsEmpty(size_t x, size_t y) const
    {
        return TileAt(x, y).IsEmpty();
    }

    bool IsMovableType(size_t x, size_t y) const
    {
        return TileAt(x, y).IsMovableType();
    }

    bool IsFixedType(size_t x, size_t y) const
    {
        return TileAt(x, y).IsFixedType();
    }

    bool IsInRun(size_t x, size_t y) const
    {
        return TileAt(x, y).IsInRun();
    }

    bool IsDescended(size_t x, size_t y) const
    {
        return TileAt(x, y).IsDescended();
    }

    void LowerHeight(size_t x, size_t y);
    void MoveDown(size_t x, size_t y);

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

    std::array<Tile, cols * rows> tiles_;
    size_t firstRow_ = 0;
    std::function<int(int, int)>& rnd_;
    bool impacted_;
    size_t run_;
    std::vector<RunInfo> runInfo_;
};
