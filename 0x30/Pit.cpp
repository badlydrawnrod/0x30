#include "Pit.h"

#include <algorithm>


#define TILE_HEIGHT 15


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


bool& Pit::RunAt(size_t x, size_t y)
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


void Pit::CheckForVerticalRun(const size_t x, const size_t y, bool& foundRun)
{
    // Check for 3 matching adjacent tiles vertically.
    int heights[] = { HeightAt(x, y), HeightAt(x, y + 1), HeightAt(x, y + 2) };
    if (heights[0] == 0 && heights[1] == 0 && heights[2] == 0)
    {
        Pit::Tile tiles[] = { TileAt(x, y), TileAt(x, y + 1), TileAt(x, y + 2) };
        if (tiles[0] == tiles[1] && tiles[1] == tiles[2])
        {
            if (tiles[0] != Pit::Tile::None && tiles[0] != Pit::Tile::Wall)
            {
                foundRun = true;
                RunAt(x, y) = true;
                RunAt(x, y + 1) = true;
                RunAt(x, y + 2) = true;
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
            CheckForVerticalRun(x, y, foundRun);
        }
    }
}


void Pit::CheckForHorizontalRun(const size_t x, const size_t y, bool& foundRun)
{
    // Check for 3 matching adjacent tiles horizontally.
    size_t row = (y + firstRow_) % rows;
    int heights[] = { HeightAt(x, y), HeightAt(x + 1, y), HeightAt(x + 2, y) };
    if (heights[0] == 0 && heights[1] == 0 && heights[2] == 0)
    {
        Pit::Tile tiles[] = { TileAt(x, y), TileAt(x + 1, y), TileAt(x + 2, y) };
        if (tiles[0] == tiles[1] && tiles[1] == tiles[2])
        {
            if (tiles[0] != Pit::Tile::None && tiles[0] != Pit::Tile::Wall)
            {
                foundRun = true;
                RunAt(x, y) = true;
                RunAt(x + 1, y) = true;
                RunAt(x + 2, y) = true;
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
            CheckForHorizontalRun(x, y, foundRun);
        }
    }
}


void Pit::CheckForAdjacentRunVertically(const size_t x, const size_t y, bool& foundRun)
{
    int heights[] = { HeightAt(x, y), HeightAt(x, y + 1) };
    if (heights[0] == 0 && heights[1] == 0)
    {
        Pit::Tile tiles[] = { TileAt(x, y), TileAt(x, y + 1) };
        bool runs[] = { RunAt(x, y), RunAt(x, y + 1) };
        if (runs[0] != runs[1] && tiles[0] == tiles[1])
        {
            if (tiles[0] != Pit::Tile::None && tiles[0] != Pit::Tile::Wall)
            {
                foundRun = true;
                RunAt(x, y) = true;
                RunAt(x, y + 1) = true;
            }
        }
    }
}


void Pit::CheckForAdjacentRunsVertically(bool& foundRun)
{
    // Look for tiles vertically adjacent to an existing run.
    for (size_t y = 0; y < rows - 2; y++)
    {
        for (size_t x = 0; x < cols; x++)
        {
            CheckForAdjacentRunVertically(x, y, foundRun);
        }
    }
}


void Pit::CheckForAdjacentRunHorizontally(const size_t x, const size_t y, bool& foundRun)
{
    int heights[] = { HeightAt(x, y), HeightAt(x + 1, y) };
    if (heights[0] == 0 && heights[1] == 0)
    {
        Pit::Tile tiles[] = { TileAt(x, y), TileAt(x + 1, y) };
        bool runs[] = { RunAt(x, y), RunAt(x + 1, y) };
        if (runs[0] != runs[1] && tiles[0] == tiles[1])
        {
            if (tiles[0] != Pit::Tile::None && tiles[0] != Pit::Tile::Wall)
            {
                foundRun = true;
                RunAt(x, y) = true;
                RunAt(x + 1, y) = true;
            }
        }
    }

}


void Pit::CheckForAdjacentRunsHorizontally(bool& foundRun)
{
    // Look for tiles horizontally adjacent to an existing run.
    for (size_t x = 0; x < cols - 1; x++)
    {
        for (size_t y = 0; y < rows; y++)
        {
            CheckForAdjacentRunHorizontally(x, y, foundRun);
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
    CheckForVerticalRuns(foundRun);
    CheckForHorizontalRuns(foundRun);

    // If we've found any horizontal or vertical runs then look for tiles of the same colour adjacent to a run and
    // add them to it, until we find no further runs.
    while (foundRun)
    {
        foundRun = false;
        CheckForAdjacentRunsVertically(foundRun);
        CheckForAdjacentRunsHorizontally(foundRun);
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
