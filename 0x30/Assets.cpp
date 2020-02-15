#include "Assets.h"

static constexpr float tile_size = 16.0f;


Textures::Textures()
{
    texture = je::LoadTextureFromFile("../assets/sprite_tiles.png");
    redTile = je::TextureRegion{ texture, 0 * tile_size, 48.0f, tile_size, tile_size };
    greenTile = je::TextureRegion{ texture, 1 * tile_size, 48.0f, tile_size, tile_size };
    yellowTile = je::TextureRegion{ texture, 2 * tile_size, 48.0f, tile_size, tile_size };
    magentaTile = je::TextureRegion{ texture, 3 * tile_size, 48.0f, tile_size, tile_size };
    cyanTile = je::TextureRegion{ texture, 4 * tile_size, 48.0f, tile_size, tile_size };
    wallTile = je::TextureRegion{ texture, 5 * tile_size, 48.0f, tile_size, tile_size };
    cursorTile = je::TextureRegion{ texture, 103.0f, 47.0f, 17.0f, 17.0f };
}
