#pragma once

#include "je/Textures.h"


struct Textures
{
    Textures();

    je::Texture texture;
    je::TextureRegion redTile;
    je::TextureRegion greenTile;
    je::TextureRegion yellowTile;
    je::TextureRegion magentaTile;
    je::TextureRegion cyanTile;
    je::TextureRegion wallTile;
    je::TextureRegion cursorTile;
};
