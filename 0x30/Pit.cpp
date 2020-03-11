#include "Pit.h"

#include "je/Logger.h"

#include <algorithm>


#define TILE_HEIGHT 15


inline size_t Pit::PitIndex(size_t x, size_t y) const
{
    size_t col = x % cols;
    size_t row = (y + firstRow_) % rows;
    return col + row * cols;
}


void Pit::PutTile(size_t x, size_t y, TileType type)
{
    auto i = PitIndex(x, y);
    tiles_[i] = type;
    heights_[i] = 0;
}


int Pit::HeightAt(size_t x, size_t y) const
{
    return heights_[PitIndex(x, y)];
}


void Pit::LowerHeight(size_t x, size_t y)
{
    auto index = PitIndex(x, y);
    --heights_[index];
}


Pit::TileType Pit::TileTypeAt(size_t x, size_t y) const
{
    return tiles_[PitIndex(x, y)];
}


inline bool Pit::IsEmpty(size_t x, size_t y) const
{
    const auto& tileType = tiles_[PitIndex(x, y)];
    return tileType == TileType::None;
}


inline bool Pit::IsMovable(size_t x, size_t y) const
{
    const auto& tileType = tiles_[PitIndex(x, y)];
    return tileType != TileType::None && tileType != TileType::Wall;
}


inline size_t& Pit::ChainAt(size_t x, size_t y)
{
    return chains_[PitIndex(x, y)];
}


inline size_t& Pit::RunAt(size_t x, size_t y)
{
    return runs_[PitIndex(x, y)];
}


inline Pit::TileType Pit::TileAt(size_t x, size_t y) const
{
    return tiles_[PitIndex(x, y)];
}


inline void Pit::MoveDown(size_t x, size_t y)
{
    size_t index = PitIndex(x, y);
    size_t indexAbove = PitIndex(x, y - 1);
    std::swap(tiles_[index], tiles_[indexAbove]);
    std::swap(chains_[index], chains_[indexAbove]);
    heights_[index] = TILE_HEIGHT;
}

Pit::Pit(std::function<int(int, int)>& rnd) : rnd_{ rnd }, impacted_{ false }, run_{ 0 }
{
    std::fill(tiles_.begin(), tiles_.end(), TileType::None);
    std::fill(heights_.begin(), heights_.end(), 0);
    std::fill(runs_.begin(), runs_.end(), 0);
    std::fill(chains_.begin(), chains_.end(), 0);
}


void Pit::RefillBottomRow()
{
    auto start = PitIndex(0, rows - 1);
    auto end = PitIndex(cols - 1, rows - 1);
    std::array<TileType, 5> pieces = {
        TileType::Cyan,
        TileType::Green,
        TileType::Magenta,
        TileType::Red,
        TileType::Yellow
    };

    int lastTile = -1;
    for (auto i = start; i <= end; i++)
    {
        // Prevent adjacent tiles from being the same colour.
        int tile = rnd_(0, 4);
        while (tile == lastTile)
        {
            tile = rnd_(0, 4);
        }
        tiles_[i] = pieces[tile];
        heights_[i] = 0;
        lastTile = tile;
    }
}


void Pit::ScrollOne()
{
    firstRow_ = (firstRow_ + 1) % rows;
    RefillBottomRow();

    // The pit is impacted if there are any tiles in the top row.
    auto start = PitIndex(0, 0);
    auto end = PitIndex(cols - 1, 0);
    for (auto i = start; i <= end; i++)
    {
        if (tiles_[i] != TileType::None)
        {
            impacted_ = true;
            break;
        }
    }
}


void Pit::Swap(size_t x, size_t y)
{
    auto& tile1 = tiles_[PitIndex(x, y)];
    auto& tile2 = tiles_[PitIndex(x + 1, y)];
    if (tile1 != Pit::TileType::Wall && tile2 != Pit::TileType::Wall)
    {
        std::swap(tile1, tile2);
    }
}


void Pit::Update()
{
    ApplyGravity();
    CheckForRuns();
    RemoveRuns();
    RemoveDeadChains();
}


void Pit::ApplyGravity()
{
    for (size_t y = rows - 2; y != 0; y--)
    {
        for (size_t x = 0; x < cols; x++)
        {
            // If the current square is empty and the one above contains a tile that is fully descended then move it
            // down to this square.
            if (IsEmpty(x, y))
            {
                if (IsMovable(x, y - 1))
                {
                    // Is it fully descended?
                    if (HeightAt(x, y - 1) == 0)
                    {
                        MoveDown(x, y);
                    }
                }
            }

            // If a tile is not fully descended then bring it down.
            if (IsMovable(x, y) && HeightAt(x, y) > 0)
            {
                LowerHeight(x, y);
            }
        }
    }
}


bool Pit::CheckForAdjacentRunVertically(const size_t x, const size_t y)
{
    // Not a run if the square underneath the run candidate is empty.
    if (IsEmpty(x, y + 2))
    {
        return false;
    }

    bool foundRun = false;
    if (HeightAt(x, y) == 0 && HeightAt(x, y + 1) == 0)
    {
        if (TileAt(x, y) == TileAt(x, y + 1))
        {
            if (RunAt(x, y) == run_ && RunAt(x, y + 1) == 0 || RunAt(x, y) == 0 && RunAt(x, y + 1) == run_)
            {
                if (IsMovable(x, y))
                {
                    foundRun = true;
                    RunAt(x, y) = run_;
                    RunAt(x, y + 1) = run_;
                }
            }
        }
    }
    return foundRun;
}


bool Pit::CheckForAdjacentRunsVertically()
{
    // Look for tiles vertically adjacent to an existing run.
    bool foundRun = false;
    for (size_t y = 0; y < rows - 2; y++)
    {
        for (size_t x = 0; x < cols; x++)
        {
            if (CheckForAdjacentRunVertically(x, y))
            {
                foundRun = true;
            }
        }
    }
    return foundRun;
}


bool Pit::CheckForAdjacentRunHorizontally(const size_t x, const size_t y)
{
    // Not a run if any of the squares underneath the run candidate are empty.
    for (size_t col = x; col < x + 2; col++)
    {
        if (IsEmpty(col, y + 1))
        {
            return false;
        }
    }

    bool foundRun = false;
    if (HeightAt(x, y) == 0 && HeightAt(x + 1, y) == 0)
    {
        if (TileAt(x, y) == TileAt(x + 1, y))
        {
            if (RunAt(x, y) == run_ && RunAt(x + 1, y) == 0 || RunAt(x, y) == 0 && RunAt(x + 1, y) == run_)
            {
                if (IsMovable(x, y))
                {
                    foundRun = true;
                    RunAt(x, y) = run_;
                    RunAt(x + 1, y) = run_;
                }
            }
        }
    }
    return foundRun;
}


bool Pit::CheckForAdjacentRunsHorizontally()
{
    // Look for tiles horizontally adjacent to an existing run.
    bool foundRun = false;
    for (size_t x = 0; x < cols - 1; x++)
    {
        for (size_t y = 0; y < rows; y++)
        {
            if (CheckForAdjacentRunHorizontally(x, y))
            {
                foundRun = true;
            }
        }
    }

    return foundRun;
}


void Pit::CheckForVerticalRun(const size_t x, const size_t y, bool& foundRun)
{
    // Not a run if the square under the run candidate is empty.
    if (IsEmpty(x, y + 3))
    {
        return;
    }

    // Not a run if there's already a run here.
    for (size_t row = y; row < y + 3; row++)
    {
        if (RunAt(x, row) > 0)
        {
            return;
        }
    }

    // Check for 3 matching adjacent tiles vertically.
    if (HeightAt(x, y) == 0 && HeightAt(x, y + 1) == 0 && HeightAt(x, y + 2) == 0)
    {
        if (TileAt(x, y) == TileAt(x, y + 1) && TileAt(x, y + 1) == TileAt(x, y + 2))
        {
            if (IsMovable(x, y))
            {
                foundRun = true;
                RunAt(x, y) = run_;
                RunAt(x, y + 1) = run_;
                RunAt(x, y + 2) = run_;
            }
        }
    }
}


void Pit::CheckForVerticalRuns(bool& foundRun)
{
    for (size_t y = 0; y < rows - 3; y++)
    {
        for (size_t x = 0; x < cols; x++)
        {
            bool isRun = false;
            CheckForVerticalRun(x, y, isRun);
            if (isRun)
            {
                for (bool moreRuns = true; moreRuns;)
                {
                    moreRuns = false;
                    if (CheckForAdjacentRunsHorizontally())
                    {
                        moreRuns = true;
                    }
                    if (CheckForAdjacentRunsVertically())
                    {
                        moreRuns = true;
                    }
                }
            }
            foundRun = foundRun || isRun;
            if (isRun)
            {
                ++run_;
            }
        }
    }
}


void Pit::CheckForHorizontalRun(const size_t x, const size_t y, bool& foundRun)
{
    // Not a run if any of the squares under the run candidate are empty or if there's already a run here.
    for (size_t col = x; col < x + 3; col++)
    {
        if (IsEmpty(col, y + 1))
        {
            return;
        }
        if (RunAt(col, y) > 0)
        {
            return;
        }
    }

    // Check for 3 matching adjacent tiles horizontally.
    if (HeightAt(x, y) == 0 && HeightAt(x + 1, y) == 0 && HeightAt(x + 2, y) == 0)
    {
        if (TileAt(x, y) == TileAt(x + 1, y) && TileAt(x + 1, y) == TileAt(x + 2, y))
        {
            if (IsMovable(x, y))
            {
                foundRun = true;
                RunAt(x, y) = run_;
                RunAt(x + 1, y) = run_;
                RunAt(x + 2, y) = run_;
            }
        }
    }
}


void Pit::CheckForHorizontalRuns(bool& foundRun)
{
    for (size_t x = 0; x < cols - 2; x++)
    {
        for (size_t y = 0; y < rows; y++)
        {
            bool isRun = false;
            CheckForHorizontalRun(x, y, isRun);
            if (isRun)
            {
                for (bool moreRuns = true; moreRuns;)
                {
                    moreRuns = false;
                    if (CheckForAdjacentRunsHorizontally())
                    {
                        moreRuns = true;
                    }
                    if (CheckForAdjacentRunsVertically())
                    {
                        moreRuns = true;
                    }
                }
            }
            foundRun = foundRun || isRun;
            if (isRun)
            {
                ++run_;
            }
        }
    }
}


void Pit::CheckForRuns()
{
    // Look for runs of tiles of the same colour that are at least 3 tiles horizontally or vertically.

    // At the start, there are no runs.
    std::fill(runs_.begin(), runs_.end(), 0);
    run_ = 1;

    // Check for 3 adacent tiles vertically and horizontally.
    bool foundRun = false;
    do
    {
        foundRun = false;
        CheckForVerticalRuns(foundRun);
        CheckForHorizontalRuns(foundRun);
    } while (foundRun);
}


void Pit::RemoveRuns()
{
    runInfo_.resize(run_ - 1);

    // There were no runs detected.
    if (run_ == 1)
    {
        return;
    }

    // Output some debug to show the pit.
    LOG("There are " << (run_ - 1) << " runs");
    for (size_t y = 0; y < rows; y++)
    {
        std::string row;
        for (size_t x = 0; x < cols; x++)
        {
            size_t run = RunAt(x, y);
            if (run > 0)
            {
                row += '0' + (int)RunAt(x, y);
            }
            else
            {
                row += '.';
            }
        }
        LOG(row);
    }

    // Find the maximum chain length for each run.
    for (size_t y = 0; y < rows; y++)
    {
        for (size_t x = 0; x < cols; x++)
        {
            if (auto run = RunAt(x, y); run > 0)
            {
                // Update the maximum chain length for this run.
                if (ChainAt(x, y) > runInfo_[run - 1].chainLength)
                {
                    runInfo_[run - 1].chainLength = ChainAt(x, y);
                }
            }
        }
    }

    // Clear all of the runs.
    for (size_t y = 0; y < rows; y++)
    {
        for (size_t x = 0; x < cols; x++)
        {
            auto index = PitIndex(x, y);
            if (auto run = RunAt(x, y); run > 0)
            {
                runInfo_[run - 1].coord[runInfo_[run - 1].runSize] = { x, y };
                ++runInfo_[run - 1].runSize;
                PutTile(x, y, TileType::None);

                // If there's a fully descended block in the row above then set its chain count to one more than the
                // maximum chain length for this run.
                if (y > 0)
                {
                    if (IsMovable(x, y - 1) && HeightAt(x, y - 1) == 0)
                    {
                        ChainAt(x, y - 1) = runInfo_[run - 1].chainLength + 1;
                    }
                }
                chains_[index] = 0;
            }
        }
    }
}


void Pit::RemoveDeadChains()
{
    for (size_t y = 0; y < rows - 1; y++)
    {
        for (size_t x = 0; x < cols; x++)
        {
            // Reset the chain if the tile we're looking at is fully descended and is blocked below.
            if (ChainAt(x, y) > 0                                   // We have a chain here.
                && IsMovable(x, y) && HeightAt(x, y) == 0           // We have a fully descended block.
                && !IsEmpty(x, y + 1) && HeightAt(x, y + 1) == 0)   // We're blocked below by a fully descended block.
            {
                ChainAt(x, y) = 0;
            }
        }
    }
}
