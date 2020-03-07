#include "Pit.h"

#include "je/Logger.h"

#include <algorithm>


#define TILE_HEIGHT 15


namespace
{
    inline bool IsMovable(const Pit::Tile& tile)
    {
        return tile != Pit::Tile::None && tile != Pit::Tile::Wall;
    }

    inline bool IsEmpty(const Pit::Tile& tile)
    {
        return tile == Pit::Tile::None;
    }
}


Pit::Pit(std::function<int(int, int)>& rnd) : rnd_{ rnd }, impacted_{ false }
{
    std::fill(tiles_.begin(), tiles_.begin() + cols * 1, Tile::Wall);
    std::fill(tiles_.begin() + cols * 1, tiles_.begin() + cols * 4, Tile::Red);
    std::fill(tiles_.begin() + cols * 4, tiles_.begin() + cols * 6, Tile::Green);
    std::fill(tiles_.begin() + cols * 6, tiles_.begin() + cols * 8, Tile::Yellow);
    std::fill(tiles_.begin() + cols * 8, tiles_.begin() + cols * 10, Tile::Cyan);
    std::fill(tiles_.begin() + cols * 10, tiles_.begin() + cols * 13, Tile::Magenta);
    for (size_t y = 0; y < rows; y++)
    {
        if (tiles_[y * cols + 3] != Tile::Wall)
        {
            tiles_[y * cols + 3] = Tile::None;
        }
        if (tiles_[y * cols + 1] != Tile::Wall)
        {
            tiles_[y * cols + 1] = Tile::None;
        }
        for (size_t x = 2; x < 5; x += 2)
        {
            switch (tiles_[y * cols + x])
            {
            case Tile::Wall:
                break;
            case Tile::Cyan:
                tiles_[y * cols + x] = Tile::Magenta;
                break;
            default:
                tiles_[y * cols + x] = Tile::Cyan;
                break;
            }
        }
    }
    std::fill(tiles_.begin() + cols * 11, tiles_.begin() + cols * 12, Tile::Red);

    std::fill(heights_.begin(), heights_.end(), 0);
    std::fill(runs_.begin(), runs_.end(), 0);
    std::fill(chains_.begin(), chains_.end(), 0);
}


void Pit::RefillBottomRow()
{
    auto start = PitIndex(0, rows - 1);
    auto end = PitIndex(cols - 1, rows - 1);
    std::array<Tile, 5> pieces = {
        Tile::Cyan,
        Tile::Green,
        Tile::Magenta,
        Tile::Red,
        Tile::Yellow
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
        if (tiles_[i] != Tile::None)
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
    if (tile1 != Pit::Tile::Wall && tile2 != Pit::Tile::Wall)
    {
        std::swap(tile1, tile2);
    }
}


size_t& Pit::RunAt(size_t x, size_t y)
{
    return runs_[PitIndex(x, y)];
}


int Pit::HeightAt(size_t x, size_t y) const
{
    return heights_[PitIndex(x, y)];
}


Pit::Tile Pit::TileAt(size_t x, size_t y) const
{
    return tiles_[PitIndex(x, y)];
}


size_t Pit::PitIndex(size_t x, size_t y) const
{
    size_t col = x % cols;
    size_t row = (y + firstRow_) % rows;
    return col + row * cols;
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
        size_t rowAbove = (y + rows - 1 + firstRow_) % rows;
        size_t row = (y + firstRow_) % rows;
        for (size_t x = 0; x < cols; x++)
        {
            // If the current square is empty and the one above contains a tile that is fully descended then move it
            // down to this square.
            if (IsEmpty(tiles_[x + row * cols]))
            {
                if (IsMovable(tiles_[x + rowAbove * cols]))
                {
                    // Is it fully descended?
                    if (const auto height = heights_[x + rowAbove * cols]; height == 0)
                    {
                        std::swap(tiles_[x + rowAbove * cols], tiles_[x + row * cols]);
                        std::swap(chains_[x + rowAbove * cols], chains_[x + row * cols]);
                        heights_[x + row * cols] = TILE_HEIGHT;
                    }
                }
            }

            // If a tile is not fully descended then bring it down.
            if (IsMovable(tiles_[x + row * cols]))
            {
                if (heights_[x + row * cols] > 0)
                {
                    --heights_[x + row * cols];
                }
            }
        }
    }
}


void Pit::CheckForAdjacentRunVertically(const size_t x, const size_t y)
{
    // Not a run if the square underneath the run candidate is empty.
    if (IsEmpty(TileAt(x, y + 2)))
    {
        return;
    }

    int heights[] = { HeightAt(x, y), HeightAt(x, y + 1) };
    if (heights[0] == 0 && heights[1] == 0)
    {
        Pit::Tile tiles[] = { TileAt(x, y), TileAt(x, y + 1) };
        if (tiles[0] == tiles[1])
        {
            size_t runs[] = { RunAt(x, y), RunAt(x, y + 1) };
            if ((runs[0] == run_ && runs[1] == 0) || (runs[0] == 0 && runs[1] == run_))
            {
                if (IsMovable(tiles[0]))
                {
                    RunAt(x, y) = run_;
                    RunAt(x, y + 1) = run_;
                }
            }
        }
    }
}


void Pit::CheckForAdjacentRunsVertically()
{
    // Look for tiles vertically adjacent to an existing run.
    for (size_t y = 0; y < rows - 2; y++)
    {
        for (size_t x = 0; x < cols; x++)
        {
            CheckForAdjacentRunVertically(x, y);
        }
    }
}


void Pit::CheckForAdjacentRunHorizontally(const size_t x, const size_t y)
{
    // Not a run if any of the squares underneath the run candidate are empty.
    for (size_t col = x; col < x + 2; col++)
    {
        if (IsEmpty(TileAt(col, y + 1)))
        {
            return;
        }
    }

    int heights[] = { HeightAt(x, y), HeightAt(x + 1, y) };
    if (heights[0] == 0 && heights[1] == 0)
    {
        Pit::Tile tiles[] = { TileAt(x, y), TileAt(x + 1, y) };
        if (tiles[0] == tiles[1])
        {
            size_t runs[] = { RunAt(x, y), RunAt(x + 1, y) };
            if ((runs[0] == run_ && runs[1] == 0) || (runs[0] == 0 && runs[1] == run_))
            {
                if (IsMovable(tiles[0]))
                {
                    RunAt(x, y) = run_;
                    RunAt(x + 1, y) = run_;
                }
            }
        }
    }

}


void Pit::CheckForAdjacentRunsHorizontally()
{
    // Look for tiles horizontally adjacent to an existing run.
    for (size_t x = 0; x < cols - 1; x++)
    {
        for (size_t y = 0; y < rows; y++)
        {
            CheckForAdjacentRunHorizontally(x, y);
        }
    }
}


void Pit::CheckForVerticalRun(const size_t x, const size_t y, bool& foundRun)
{
    // Not a run if the square under the run candidate is empty.
    if (IsEmpty(TileAt(x, y + 3)))
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
    int heights[] = { HeightAt(x, y), HeightAt(x, y + 1), HeightAt(x, y + 2) };
    if (heights[0] == 0 && heights[1] == 0 && heights[2] == 0)
    {
        Pit::Tile tiles[] = { TileAt(x, y), TileAt(x, y + 1), TileAt(x, y + 2) };
        if (tiles[0] == tiles[1] && tiles[1] == tiles[2])
        {
            if (IsMovable(tiles[0]))
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
                CheckForAdjacentRunsHorizontally();
                CheckForAdjacentRunsVertically();
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
        if (IsEmpty(TileAt(col, y + 1)))
        {
            return;
        }
        if (RunAt(col, y) > 0)
        {
            return;
        }
    }

    // Check for 3 matching adjacent tiles horizontally.
    size_t row = (y + firstRow_) % rows;
    int heights[] = { HeightAt(x, y), HeightAt(x + 1, y), HeightAt(x + 2, y) };
    if (heights[0] == 0 && heights[1] == 0 && heights[2] == 0)
    {
        Pit::Tile tiles[] = { TileAt(x, y), TileAt(x + 1, y), TileAt(x + 2, y) };
        if (tiles[0] == tiles[1] && tiles[1] == tiles[2])
        {
            if (IsMovable(tiles[0]))
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
                CheckForAdjacentRunsHorizontally();
                CheckForAdjacentRunsVertically();
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
            auto index = PitIndex(x, y);
            if (auto run = runs_[index]; run > 0)
            {
                // Update the maximum chain length for this run.
                if (1 + chains_[index] > runInfo_[run - 1].chainLength)
                {
                    runInfo_[run - 1].chainLength = 1 + chains_[index];
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
            if (auto run = runs_[index]; run > 0)
            {
                runInfo_[run - 1].coord[runInfo_[run - 1].runSize] = { x, y };
                ++runInfo_[run - 1].runSize;
                tiles_[index] = Pit::Tile::None;
                heights_[index] = 0;

                // If there's a fully descended block in the row above then set its chain count to one more than the
                // maximum chain length for this run.
                auto previousRow = PitIndex(x, y - 1);
                if (IsMovable(tiles_[previousRow]) && heights_[previousRow] == 0)
                {
                    chains_[previousRow] = runInfo_[run - 1].chainLength + 1;
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
        size_t rowBelow = (y + rows + 1 + firstRow_) % rows;
        size_t row = (y + firstRow_) % rows;
        for (size_t x = 0; x < cols; x++)
        {
            // Reset the chain if the tile we're looking at is fully descended and is blocked below.
            if (chains_[x + row * cols] > 0                                                     // We have a chain here.
                && IsMovable(tiles_[x + row * cols]) && heights_[x + row * cols] == 0           // We have a fully descended block.
                && !IsEmpty(tiles_[x + rowBelow * cols]) && heights_[x + rowBelow * cols] == 0) // We're blocked below by a fully descended block.
            {
                chains_[x + row * cols] = 0;
            }
        }
    }
}
