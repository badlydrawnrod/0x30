#include "Assets.h"

#include <filesystem>


static constexpr float tile_size = 16.0f;


Textures::Textures()
{
    texture = je::LoadTextureFromFile("../assets/sprite_tiles.png");

    blankTile = je::TextureRegion{ texture, 0, 32.0f, tile_size, tile_size };

    redTile = je::TextureRegion{ texture, 0 * tile_size, 48.0f, tile_size, tile_size };
    greenTile = je::TextureRegion{ texture, 1 * tile_size, 48.0f, tile_size, tile_size };
    yellowTile = je::TextureRegion{ texture, 2 * tile_size, 48.0f, tile_size, tile_size };
    magentaTile = je::TextureRegion{ texture, 3 * tile_size, 48.0f, tile_size, tile_size };
    cyanTile = je::TextureRegion{ texture, 4 * tile_size, 48.0f, tile_size, tile_size };
    wallTile = je::TextureRegion{ texture, 5 * tile_size, 48.0f, tile_size, tile_size };
    cursorTile = je::TextureRegion{ texture, 103.0f, 47.0f, 17.0f, 17.0f };

    textTiles = je::TextureRegion{ texture, 0.0f, 80.0f, static_cast<GLfloat>(texture.w), static_cast<GLfloat>(texture.h) - 80.0f };

    pitTopLeft = je::TextureRegion{ texture, 0.0f, 0.0f, 8.0f, 8.0f };
    pitTop = je::TextureRegion{ texture, 8.0f, 0.0f, 8.0f, 8.0f };
    pitTopRight = je::TextureRegion{ texture, 16.0f, 0.0f, 8.0f, 8.0f };
    pitLeft = je::TextureRegion{ texture, 0.0f, 8.0f, 8.0f, 8.0f };
    pitRight = je::TextureRegion{ texture, 16.0f, 8.0f, 8.0f, 8.0f };
    pitBottomLeft = je::TextureRegion{ texture, 0.0f, 16.0f, 8.0f, 8.0f };
    pitBottom = je::TextureRegion{ texture, 8.0f, 16.0f, 8.0f, 8.0f };
    pitBottomRight = je::TextureRegion{ texture, 16.0f, 16.0f, 8.0f, 8.0f };

    combo4 = je::TextureRegion{ texture,  80.0f, 0.0f, 8.0f, 8.0f };
    combo5 = je::TextureRegion{ texture,  88.0f, 0.0f, 8.0f, 8.0f };
    combo6 = je::TextureRegion{ texture,  96.0f, 0.0f, 8.0f, 8.0f };
    combo7 = je::TextureRegion{ texture, 104.0f, 0.0f, 8.0f, 8.0f };
    combo8 = je::TextureRegion{ texture, 112.0f, 0.0f, 8.0f, 8.0f };
    combo9 = je::TextureRegion{ texture, 120.0f, 0.0f, 8.0f, 8.0f };

    chain2 = je::TextureRegion{ texture,  40.0f, 24.0f, 12.0f, 8.0f };
    chain3 = je::TextureRegion{ texture,  56.0f, 24.0f, 12.0f, 8.0f };
    chain4 = je::TextureRegion{ texture,  72.0f, 24.0f, 12.0f, 8.0f };
    chain5 = je::TextureRegion{ texture,  88.0f, 24.0f, 12.0f, 8.0f };
    chain6 = je::TextureRegion{ texture, 104.0f, 24.0f, 12.0f, 8.0f };

    std::filesystem::path backdropsDir{ "../assets/backdrops" };
    for (auto& entry : std::filesystem::directory_iterator(backdropsDir))
    {
        std::filesystem::path path = entry.path();
        std::string pathString = path.string();
        const char* filename = pathString.data();
        backdrops.push_back(je::LoadTextureFromFile(filename));
    }
}
