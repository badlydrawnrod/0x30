#include "Pit.h"

#include <algorithm>


#define TILE_HEIGHT 15


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
    std::fill(runs_.begin(), runs_.end(), false);
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
            if (tiles_[x + row * cols] == Pit::Tile::None)
            {
                const auto tile = tiles_[x + rowAbove * cols];
                if (tile != Pit::Tile::None && tile != Pit::Tile::Wall)
                {
                    // Is it fully descended?
                    if (const auto height = heights_[x + rowAbove * cols]; height == 0)
                    {
                        std::swap(tiles_[x + rowAbove * cols], tiles_[x + row * cols]);
                        heights_[x + row * cols] = TILE_HEIGHT;
                    }
                }
            }

            // If a tile is not fully descended then bring it down.
            const auto tile = tiles_[x + row * cols];
            if (tile != Pit::Tile::None && tile != Pit::Tile::Wall)
            {
                if (heights_[x + row * cols] > 0)
                {
                    --heights_[x + row * cols];
                }
            }
        }
    }
}


void Pit::FindVerticalRuns(bool& foundRun)
{
    for (size_t y = 0; y < rows - 3; y++)
    {
        size_t row0 = (y + firstRow_) % rows;
        size_t row1 = (row0 + 1) % rows;
        size_t row2 = (row0 + 2) % rows;
        for (size_t x = 0; x < cols; x++)
        {
            auto tile0 = tiles_[x + row0 * cols];
            auto tile1 = tiles_[x + row1 * cols];
            auto tile2 = tiles_[x + row2 * cols];
            if (tile0 == tile1 && tile1 == tile2)
            {
                if (tile0 != Pit::Tile::None && tile0 != Pit::Tile::Wall)
                {
                    foundRun = true;
                    runs_[x + row0 * cols] = true;
                    runs_[x + row1 * cols] = true;
                    runs_[x + row2 * cols] = true;
                }
            }
        }
    }
}


void Pit::FindHorizontalRuns(bool& foundRun)
{
    // Check for 3 adjacent tiles horizontally.
    for (size_t x = 0; x < cols - 2; x++)
    {
        for (size_t y = 0; y < rows; y++)
        {
            size_t row = (y + firstRow_) % rows;
            auto tile0 = tiles_[(x + 0) + row * cols];
            auto tile1 = tiles_[(x + 1) + row * cols];
            auto tile2 = tiles_[(x + 2) + row * cols];
            if (tile0 == tile1 && tile1 == tile2)
            {
                if (tile0 != Pit::Tile::None && tile0 != Pit::Tile::Wall)
                {
                    foundRun = true;
                    runs_[(x + 0) + row * cols] = true;
                    runs_[(x + 1) + row * cols] = true;
                    runs_[(x + 2) + row * cols] = true;
                }
            }
        }
    }
}


void Pit::FindAdjacentVerticalRuns(bool& foundRun)
{
    // Look for tiles vertically adjacent to an existing run.
    for (size_t y = 0; y < rows - 2; y++)
    {
        size_t row0 = (y + firstRow_) % rows;
        size_t row1 = (row0 + 1) % rows;
        for (size_t x = 0; x < cols; x++)
        {
            auto tile0 = tiles_[x + row0 * cols];
            auto tile1 = tiles_[x + row1 * cols];
            auto run0 = runs_[x + row0 * cols];
            auto run1 = runs_[x + row1 * cols];
            if (run0 != run1 && tile0 == tile1)
            {
                if (tile0 != Pit::Tile::None && tile0 != Pit::Tile::Wall)
                {
                    foundRun = true;
                    runs_[x + row0 * cols] = true;
                    runs_[x + row1 * cols] = true;
                }
            }
        }
    }
}


void Pit::FindAdjacentHorizontalRuns(bool& foundRun)
{
    // Look for tiles horizontally adjacent to an existing run.
    for (size_t x = 0; x < cols - 1; x++)
    {
        for (size_t y = 0; y < rows; y++)
        {
            size_t row = (y + firstRow_) % rows;
            auto tile0 = tiles_[(x + 0) + row * cols];
            auto tile1 = tiles_[(x + 1) + row * cols];
            auto run0 = runs_[(x + 0) + row * cols];
            auto run1 = runs_[(x + 1) + row * cols];
            if (run0 != run1 && tile0 == tile1)
            {
                if (tile0 != Pit::Tile::None && tile0 != Pit::Tile::Wall)
                {
                    foundRun = true;
                    runs_[(x + 0) + row * cols] = true;
                    runs_[(x + 1) + row * cols] = true;
                }
            }
        }
    }
}


void Pit::CheckForRuns()
{
    // Look for runs of tiles of the same colour that are at least 3 tiles horizontally or vertically.
    // TODO: Don't include falling tiles, i.e., those that have a blank square below them.

    // At the start, there are no runs.
    std::fill(runs_.begin(), runs_.end(), false);

    // Check for 3 adacent tiles vertically and horizontally.
    bool foundRun = false;
    FindVerticalRuns(foundRun);
    FindHorizontalRuns(foundRun);

    // If we've found any horizontal or vertical runs then look for tiles of the same colour adjacent to a run and
    // add them to it, until we find no further runs.
    while (foundRun)
    {
        foundRun = false;
        FindAdjacentVerticalRuns(foundRun);
        FindAdjacentHorizontalRuns(foundRun);
    }
}


void Pit::RemoveRuns()
{
    for (size_t y = 0; y < rows; y++)
    {
        size_t row = (y + firstRow_) % rows;
        for (size_t x = 0; x < cols; x++)
        {
            if (runs_[x + row * cols])
            {
                tiles_[x + row * cols] = Pit::Tile::None;
                heights_[x + row * cols] = 0;
            }
        }
    }
}
