#include "Pit.h"

#include "je/Logger.h"

#include <algorithm>


#define TILE_HEIGHT 15


void Pit::LowerHeight(size_t x, size_t y)
{
    auto& tile = TileAt(x, y);
    --tile.height;
}


inline void Pit::MoveDown(size_t x, size_t y)
{
    std::swap(TileAt(x, y), TileAt(x, y - 1));
    TileAt(x, y).height = TILE_HEIGHT;
}


Pit::Pit(std::function<int(int, int)>& rnd) : rnd_{ rnd }, impacted_{ false }, run_{ 0 }
{
    std::fill(tiles_.begin(), tiles_.end(), Tile());
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
        tiles_[i] = Tile(pieces[tile]);
        lastTile = tile;
    }
}


void Pit::ScrollOne()
{
    firstRow_ = (firstRow_ + 1) % rows;
    RefillBottomRow();

    // The pit is impacted if there are any non-empty tiles in the top row.
    auto start = PitIndex(0, 0);
    auto end = PitIndex(cols - 1, 0);
    for (auto i = start; i <= end; i++)
    {
        if (!tiles_[i].IsEmpty())
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
    if (tile1.IsMovableType() && tile2.IsMovableType())
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
                if (IsMovableType(x, y - 1) && IsDescended(x, y - 1))
                {
                    MoveDown(x, y);
                }
            }

            // If a tile is not fully descended then bring it down.
            if (IsMovableType(x, y) && !IsDescended(x, y))
            {
                LowerHeight(x, y);
            }
        }
    }
}


void Pit::AddRun(size_t x, size_t y)
{
    currentRun_.Add(x, y);
    RunAt(x, y) = run_;
}


bool Pit::CheckForAdjacentRunVertically(const size_t x, const size_t y)
{
    // Not a run if the square underneath the run candidate is empty.
    if (IsEmpty(x, y + 2))
    {
        return false;
    }

    bool foundRun = false;
    if (IsDescended(x, y) && IsDescended(x, y + 1))
    {
        if (TileTypeAt(x, y) == TileTypeAt(x, y + 1) && IsMovableType(x, y) && !IsEmpty(x, y))
        {
            if (RunAt(x, y) == run_ && RunAt(x, y + 1) == 0 || RunAt(x, y) == 0 && RunAt(x, y + 1) == run_)
            {
                foundRun = true;
                AddRun(x, y);
                AddRun(x, y + 1);
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
    if (IsDescended(x, y) && IsDescended(x + 1, y))
    {
        if (TileTypeAt(x, y) == TileTypeAt(x + 1, y) && IsMovableType(x, y) && !IsEmpty(x, y))
        {
            if (RunAt(x, y) == run_ && RunAt(x + 1, y) == 0 || RunAt(x, y) == 0 && RunAt(x + 1, y) == run_)
            {
                foundRun = true;
                AddRun(x, y);
                AddRun(x + 1, y);
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


bool Pit::CheckForVerticalRun(const size_t x, const size_t y)
{
    // Not a run if the square under the run candidate is empty.
    if (IsEmpty(x, y + 3))
    {
        return false;
    }

    // Not a run if there's already a run here.
    for (size_t row = y; row < y + 3; row++)
    {
        if (RunAt(x, row) > 0)
        {
            return false;
        }
    }

    // Check for 3 matching adjacent tiles vertically.
    bool foundRun = false;
    if (IsDescended(x, y) && IsDescended(x, y + 1) && IsDescended(x, y + 2))
    {
        if (TileTypeAt(x, y) == TileTypeAt(x, y + 1) && TileTypeAt(x, y + 1) == TileTypeAt(x, y + 2))
        {
            if (IsMovableType(x, y) && !IsEmpty(x, y))
            {
                foundRun = true;
                AddRun(x, y);
                AddRun(x, y + 1);
                AddRun(x, y + 2);
            }
        }
    }

    return foundRun;
}


bool Pit::CheckForVerticalRuns()
{
    bool foundRun = false;
    for (size_t y = 0; y < rows - 3; y++)
    {
        for (size_t x = 0; x < cols; x++)
        {
            currentRun_.Reset();
            if (CheckForVerticalRun(x, y))
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
                foundRun = true;
                ++run_;
            }
        }
    }
    
    return foundRun;
}


bool Pit::CheckForHorizontalRun(const size_t x, const size_t y)
{
    // Not a run if any of the squares under the run candidate are empty or if there's already a run here.
    for (size_t col = x; col < x + 3; col++)
    {
        if (IsEmpty(col, y + 1))
        {
            return false;
        }
        if (RunAt(col, y) > 0)
        {
            return false;
        }
    }

    // Check for 3 matching adjacent tiles horizontally.
    bool foundRun = false;

    if (IsDescended(x, y) && IsDescended(x + 1, y) && IsDescended(x + 2, y))
    {
        if (TileTypeAt(x, y) == TileTypeAt(x + 1, y) && TileTypeAt(x + 1, y) == TileTypeAt(x + 2, y))
        {
            if (IsMovableType(x, y) && !IsEmpty(x, y))
            {
                foundRun = true;
                AddRun(x, y);
                AddRun(x + 1, y);
                AddRun(x + 2, y);
            }
        }
    }

    return foundRun;
}


bool Pit::CheckForHorizontalRuns()
{
    bool foundRun = false;
    for (size_t x = 0; x < cols - 2; x++)
    {
        for (size_t y = 0; y < rows; y++)
        {
            currentRun_.Reset();
            if (CheckForHorizontalRun(x, y))
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
                foundRun = true;
                ++run_;
            }
        }
    }

    return foundRun;
}


void Pit::CheckForRuns()
{
    // Look for runs of tiles of the same colour that are at least 3 tiles horizontally or vertically.

    // At the start, there are no runs.
    for (auto& tile : tiles_)
    {
        tile.runId = 0;
    }
    run_ = 1;

    // Check for 3 adacent tiles vertically and horizontally.
    bool foundRun;
    do
    {
        foundRun = false;
        if (CheckForVerticalRuns())
        {
            foundRun = true;
        }
        if (CheckForHorizontalRuns())
        {
            foundRun = true;
        }
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
                runInfo_[run - 1].coord.push_back(PitCoord{ x, y });
                ++runInfo_[run - 1].runSize;
                ClearTile(x, y);

                // If there's a fully descended block in the row above then set its chain count to one more than the
                // maximum chain length for this run.
                if (y > 0)
                {
                    if (IsMovableType(x, y - 1) && IsDescended(x, y - 1))
                    {
                        ChainAt(x, y - 1) = runInfo_[run - 1].chainLength + 1;
                    }
                }
                tiles_[index].chain = 0;
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
                && IsMovableType(x, y) && IsDescended(x, y)         // We have a fully descended block.
                && !IsEmpty(x, y + 1) && IsDescended(x, y + 1))     // We're blocked below by a fully descended block.
            {
                ChainAt(x, y) = 0;
            }
        }
    }
}
