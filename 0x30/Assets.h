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
    je::TextureRegion textTiles;
    je::TextureRegion pitTopLeft;
    je::TextureRegion pitTop;
    je::TextureRegion pitTopRight;
    je::TextureRegion pitLeft;
    je::TextureRegion pitRight;
    je::TextureRegion pitBottomLeft;
    je::TextureRegion pitBottom;
    je::TextureRegion pitBottomRight;
};
