#include "Pit.h"

#include "je/Logger.h"

#include <algorithm>

#define TILE_HEIGHT 15

int Pit::LowerHeight(size_t x, size_t y)
{
    auto& tile = TileAt(x, y);
    return --tile.height;
}

inline void Pit::MoveDown(size_t x, size_t y)
{
    std::swap(TileAt(x, y), TileAt(x, y - 1));
    TileAt(x, y).height = TILE_HEIGHT;
}

Pit::Pit(std::function<int(int, int)>& rnd)
    : rnd_{rnd}, impacted_{false}, run_{0}
{
    std::fill(tiles_.begin(), tiles_.end(), Tile());
}

void Pit::SetLevel(size_t level)
{
    level_ = level;
}

void Pit::Reset(size_t level)
{
    level_ = level;
    std::fill(tiles_.begin(), tiles_.end(), Tile());
    RefillRows(rows / 2);
    run_ = 0;
    impacted_ = false;
    landed_ = false;
    runInfo_.clear();
}

void Pit::Refill(size_t row)
{
    const std::array<TileType, 6> pieces = {
            TileType::Red,
            TileType::Yellow,
            TileType::Cyan,
            TileType::Magenta,
            TileType::Green,
            TileType::Blue};

    auto start = PitIndex(0, row);
    auto end = PitIndex(cols - 1, row);
    auto above = PitIndex(0, row - 2);

    int lastTile = -1;
    int maxTile = 0;
    if (level_ <= 3)// Levels 1-3 have 3 tile types.
    {
        maxTile = 2;
    }
    else if (level_ <= 8)// Levels 4-8 have 4 tile types.
    {
        maxTile = 3;
    }
    else if (level_ <= 15)// Levels 9-15 have 5 tile types.
    {
        maxTile = 4;
    }
    else// Levels 16-20 have 6 tile types.
    {
        maxTile = 5;
    }
    for (auto i = start; i <= end; i++, above++)
    {
        // Prevent adjacent tiles from being the same colour.
        const TileType tileAbove = tiles_[above].tileType;
        int tile = rnd_(0, maxTile);
        while (tile == lastTile || pieces[tile] == tileAbove)
        {
            tile = rnd_(0, maxTile);
        }
        tiles_[i] = Tile(pieces[tile]);
        lastTile = tile;
    }
}

void Pit::RefillRows(int numRows)
{
    for (size_t row = rows - numRows; row != rows; row++)
    {
        Refill(row);
    }
}

void Pit::RefillBottomRow()
{
    RefillRows(1);
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
    bool landed = false;
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
            if (!IsEmpty(x, y) && IsMovableType(x, y) && !IsDescended(x, y))
            {
                // Did the tile just fully descend onto a non-empty tile?
                if (LowerHeight(x, y) == 0 && !IsEmpty(x, y + 1))
                {
                    // If the non-empty tile is either not movable, or is descended itself, then the tile just landed.
                    if (!IsMovableType(x, y + 1) || IsDescended(x, y + 1))
                    {
                        landed = true;
                    }
                }
            }
        }
    }
    landed_ = landed;
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
            if (RunAt(x, y) == (run_ && RunAt(x, y + 1) == 0) || (RunAt(x, y) == 0 && RunAt(x, y + 1) == run_))
            {
                foundRun = true;
                RunAt(x, y) = run_;
                RunAt(x, y + 1) = run_;
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
            if (RunAt(x, y) == (run_ && RunAt(x + 1, y) == 0) || (RunAt(x, y) == 0 && RunAt(x + 1, y) == run_))
            {
                foundRun = true;
                RunAt(x, y) = run_;
                RunAt(x + 1, y) = run_;
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
                RunAt(x, y) = run_;
                RunAt(x, y + 1) = run_;
                RunAt(x, y + 2) = run_;
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
                RunAt(x, y) = run_;
                RunAt(x + 1, y) = run_;
                RunAt(x + 2, y) = run_;
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
                row += '0' + (char)RunAt(x, y);
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
                runInfo_[run - 1].coord.push_back(PitCoord{x, y});
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
            if (ChainAt(x, y) > 0                              // We have a chain here.
                && IsMovableType(x, y) && IsDescended(x, y)    // We have a fully descended block.
                && !IsEmpty(x, y + 1) && IsDescended(x, y + 1))// We're blocked below by a fully descended block.
            {
                ChainAt(x, y) = 0;
            }
        }
    }
}
