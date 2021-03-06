#pragma once

#include "je/Textures.h"

#include <vector>

struct Textures
{
    Textures();

    je::Texture texture;
    std::vector<je::Texture> backdrops;

    je::TextureRegion blankSquare;
    je::TextureRegion whiteSquare;

    je::TextureRegion redTile;
    je::TextureRegion greenTile;
    je::TextureRegion yellowTile;
    je::TextureRegion magentaTile;
    je::TextureRegion cyanTile;
    je::TextureRegion blueTile;

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

    je::TextureRegion combo4;
    je::TextureRegion combo5;
    je::TextureRegion combo6;
    je::TextureRegion combo7;
    je::TextureRegion combo8;
    je::TextureRegion combo9;

    je::TextureRegion chain2;
    je::TextureRegion chain3;
    je::TextureRegion chain4;
    je::TextureRegion chain5;
    je::TextureRegion chain6;
};
